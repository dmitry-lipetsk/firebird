 /*
 *  The contents of this file are subject to the Initial
 *  Developer's Public License Version 1.0 (the "License");
 *  you may not use this file except in compliance with the
 *  License. You may obtain a copy of the License at
 *  http://www.ibphoenix.com/main.nfs?a=ibphoenix&page=ibp_idpl.
 *
 *  Software distributed under the License is distributed AS IS,
 *  WITHOUT WARRANTY OF ANY KIND, either express or implied.
 *  See the License for the specific language governing rights
 *  and limitations under the License.
 *
 *  The Original Code was created by Dmitry Yemanov
 *  for the Firebird Open Source RDBMS project.
 *
 *  Copyright (c) 2013 Dmitry Yemanov <dimitr@firebirdsql.org>
 *  and all contributors signed below.
 *
 *  All Rights Reserved.
 *  Contributor(s): ______________________________________.
 */


#ifndef JRD_REPLICATION_REPLICATOR_H
#define JRD_REPLICATION_REPLICATOR_H

#include "../common/classes/timestamp.h"
#include "../common/classes/MetaString.h"
#include "../common/os/guid.h"
#include "../jrd/align.h"
#include "../jrd/status.h"

#include "Protocol.h"
#include "Manager.h"

namespace Replication
{
	class Replicator :
		public Firebird::StdPlugin<Firebird::IReplicatedSessionImpl<Replicator, Firebird::CheckStatusWrapper> >
	{
		typedef Firebird::Array<Firebird::MetaString> MetadataCache;
		typedef Firebird::HalfStaticArray<SavNumber, 16> SavepointStack;

		struct BatchBlock
		{
			Block header;
			Firebird::UCharBuffer* buffer;
			MetadataCache metadata;
			ULONG lastMetaId;
			ULONG flushes;

			explicit BatchBlock(MemoryPool& pool)
				: buffer(NULL), metadata(pool),
				  lastMetaId(MAX_ULONG), flushes(0)
			{
				memset(&header, 0, sizeof(Block));
			}

			ULONG getSize() const
			{
				return (ULONG) buffer->getCount();
			}

			void putTag(UCHAR tag)
			{
				buffer->add(tag);
			}

			void putInt(SLONG value)
			{
				const auto newSize = FB_ALIGN(getSize(), type_alignments[dtype_long]);
				buffer->resize(newSize);
				const auto ptr = (const UCHAR*) &value;
				buffer->add(ptr, sizeof(SLONG));
			}

			void putBigInt(SINT64 value)
			{
				const auto newSize = FB_ALIGN(getSize(), type_alignments[dtype_int64]);
				buffer->resize(newSize);
				const auto ptr = (const UCHAR*) &value;
				buffer->add(ptr, sizeof(SINT64));
			}

			void putMetaName(const Firebird::MetaString& name)
			{
				if (lastMetaId < metadata.getCount() && metadata[lastMetaId] == name)
				{
					putInt(lastMetaId);
					return;
				}

				FB_SIZE_T pos;
				if (!metadata.find(name, pos))
				{
					pos = metadata.getCount();
					metadata.add(name);
				}

				putInt(pos);
				lastMetaId = (ULONG) pos;
			}

			void putString(const Firebird::string& str)
			{
				const auto length = str.length();
				putInt(length);
				buffer->add((const UCHAR*) str.c_str(), length);
			}

			void putBinary(ULONG length, const UCHAR* data)
			{
				putInt(length);
				buffer->add(data, length);
			}
		};

		class Transaction :
			public Firebird::AutoIface<Firebird::IReplicatedTransactionImpl<Transaction, Firebird::CheckStatusWrapper> >
		{
		public:
			explicit Transaction(Replicator* replicator, Firebird::ITransaction* trans)
				: m_replicator(replicator), m_transaction(trans), m_data(replicator->getPool())
			{}

			BatchBlock& getData()
			{
				return m_data;
			}

			Firebird::ITransaction* getInterface()
			{
				return m_transaction.getPtr();
			}

			// IDisposable methods

			void dispose() override
			{
				m_replicator->releaseTransaction(this);
				delete this;
			}

			// IReplicatedTransaction methods

			void prepare(Firebird::CheckStatusWrapper* status)
			{
				m_replicator->prepareTransaction(status, this);
			}

			void commit(Firebird::CheckStatusWrapper* status)
			{
				m_replicator->commitTransaction(status, this);
			}

			void rollback(Firebird::CheckStatusWrapper* status)
			{
				m_replicator->rollbackTransaction(status, this);
			}

			void startSavepoint(Firebird::CheckStatusWrapper* status)
			{
				m_replicator->startSavepoint(status, this);
			}

			void releaseSavepoint(Firebird::CheckStatusWrapper* status)
			{
				m_replicator->releaseSavepoint(status, this);
			}

			void rollbackSavepoint(Firebird::CheckStatusWrapper* status)
			{
				m_replicator->rollbackSavepoint(status, this);
			}

			void insertRecord(Firebird::CheckStatusWrapper* status, const char* name,
							  Firebird::IReplicatedRecord* record)
			{
				m_replicator->insertRecord(status, this, name, record);
			}

			void updateRecord(Firebird::CheckStatusWrapper* status, const char* name,
							  Firebird::IReplicatedRecord* orgRecord,
							  Firebird::IReplicatedRecord* newRecord)
			{
				m_replicator->updateRecord(status, this, name, orgRecord, newRecord);
			}

			void deleteRecord(Firebird::CheckStatusWrapper* status, const char* name,
							  Firebird::IReplicatedRecord* record)
			{
				m_replicator->deleteRecord(status, this, name, record);
			}

			void executeSql(Firebird::CheckStatusWrapper* status, const char* sql)
			{
				m_replicator->executeSql(status, this, sql);
			}

			void executeSqlIntl(Firebird::CheckStatusWrapper* status, unsigned charset, const char* sql)
			{
				m_replicator->executeSqlIntl(status, this, charset, sql);
			}

		private:
			Replicator* const m_replicator;
			Firebird::RefPtr<Firebird::ITransaction> m_transaction;
			BatchBlock m_data;
		};

		struct GeneratorValue
		{
			Jrd::MetaName name;
			SINT64 value;
		};

		typedef Firebird::Array<GeneratorValue> GeneratorCache;

		enum FlushReason
		{
			FLUSH_OVERFLOW,
			FLUSH_PREPARE,
			FLUSH_SYNC
		};

	public:
		Replicator(Firebird::MemoryPool& pool,
				   Manager* manager,
				   const Firebird::Guid& dbGuid,
				   const Firebird::MetaString& userName);

		// IReplicatedSession methods

		void setAttachment(Firebird::IAttachment* att) override
		{
			m_attachment = att;
		}

		Firebird::IReplicatedTransaction* startTransaction(Firebird::CheckStatusWrapper* status, Firebird::ITransaction* trans, SINT64 number);
		void cleanupTransaction(Firebird::CheckStatusWrapper* status, SINT64 number);
		void setSequence(Firebird::CheckStatusWrapper* status, const char* name, SINT64 value);

	private:
		Manager* const m_manager;
		const Config* const m_config;
		Firebird::Guid m_guid;
		const Firebird::MetaString m_user;
		Firebird::Array<Transaction*> m_transactions;
		GeneratorCache m_generators;
		Firebird::Mutex m_mutex;
		Firebird::RefPtr<Firebird::IAttachment> m_attachment;

		void initialize();
		void flush(BatchBlock& txnData, FlushReason reason, ULONG flags = 0);
		// Blob id is passed by value because BlobWrapper requires reference to non-const ISC_QUAD
		void storeBlob(Transaction* transaction, ISC_QUAD blobId);
		void releaseTransaction(Transaction* transaction);

		void prepareTransaction(Firebird::CheckStatusWrapper* status, Transaction* transaction);
		void commitTransaction(Firebird::CheckStatusWrapper* status, Transaction* transaction);
		void rollbackTransaction(Firebird::CheckStatusWrapper* status, Transaction* transaction);

		void startSavepoint(Firebird::CheckStatusWrapper* status, Transaction* transaction);
		void releaseSavepoint(Firebird::CheckStatusWrapper* status, Transaction* transaction);
		void rollbackSavepoint(Firebird::CheckStatusWrapper* status, Transaction* transaction);

		void insertRecord(Firebird::CheckStatusWrapper* status, Transaction* transaction,
						  const char* name,
						  Firebird::IReplicatedRecord* record);
		void updateRecord(Firebird::CheckStatusWrapper* status, Transaction* transaction,
						  const char* name,
						  Firebird::IReplicatedRecord* orgRecord,
						  Firebird::IReplicatedRecord* newRecord);
		void deleteRecord(Firebird::CheckStatusWrapper* status, Transaction* transaction,
						  const char* name,
						  Firebird::IReplicatedRecord* record);

		void executeSql(Firebird::CheckStatusWrapper* status, Transaction* transaction,
						const char* sql)
		{
			executeSqlIntl(status, transaction, CS_UTF8, sql);
		}

		void executeSqlIntl(Firebird::CheckStatusWrapper* status, Transaction* transaction,
							unsigned charset,
							const char* sql);
};

} // namespace

#endif // JRD_REPLICATION_REPLICATOR_H