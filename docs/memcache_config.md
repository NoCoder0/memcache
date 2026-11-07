# MemCache Configs

## MetaService Config

| key                                                  | value type | requirement | default                  | valid range                | description                                                                                                                                                             |
|------------------------------------------------------|------------|-------------|--------------------------|----------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| ock.mmc.meta_service_url                             | string     | optional    | tcp://127.0.0.1:5000     | tcp://\<host>\<port>       | host supports ip and domain, port in [1025, 65535]                                                                                                                      |
| ock.mmc.meta_service.config_store_url                | string     | optional    | tcp://127.0.0.1:6000     | tcp://\<host>\<port>       | host supports ip and domain, port in [1025, 65535]                                                                                                                      |
| ock.mmc.meta_service.metrics_url                     | string     | optional    | http://127.0.0.1:8000    | \<protocol>://\<host>:\<port>| protocol is http or https, host supports ip and domain, port in [1025, 65535]                                                                                         |
| ock.mmc.meta.ha.enable                               | bool       | optional    | false                    | true/false                 | enable meta service master/backup HA in k8s cluster                                                                                                                     |
| ock.mmc.meta_service.metrics_report_interval_seconds | integer    | optional    | 30                       | [0, 86400]                 | metrics summary printing interval in seconds, 0 disables periodic metrics printing                                                                                      |
| ock.mmc.meta.lease_ttl_ms                            | integer    | optional    | 10000                    | [1, 2147483647]            | default read lease TTL in milliseconds used when meta service grants read leases                                                                                        |
| ock.mmc.log_level                                    | string     | optional    | info                     | debug/info/warn/error      | log level                                                                                                                                                               |
| ock.mmc.log_path                                     | string     | optional    | /var/log/memcache_hybrid | relative or absolute path  | log path, the absolute path is start with '/'                                                                                                                           |
| ock.mmc.log_rotation_file_size                       | int        | optional    | 20                       | 1 <= n <= 500              | log rotation file size(MB)                                                                                                                                              |
| ock.mmc.log_rotation_file_count                      | int        | optional    | 50                       | 1 <= n <= 50               | log rotation file num                                                                                                                                                   |
| ock.mmc.log_output_target                            | string     | optional    | file                     | screen/file/both           | log output target: screen (stdout only), file (rotating file only), both (stdout + file)                                                                                |
| ock.mmc.evict_threshold_high                         | int        | optional    | 90                       | 1 <= n <= 99               | evict threshold, 90 mean 90%, the max threshold is 99%. Note: When the size of single put value is greater than 1% of the capacity, the elimination cannot be triggered |
| ock.mmc.evict_threshold_low                          | int        | optional    | 80                       | 1 <= n <= 98               | after evict threshold                                                                                                                                                   |
| ock.mmc.storage.prefetch.enabled                     | bool       | optional    | true                     | true/false                 | enable SSD to DRAM prefetch on read                                                                                                                                     |
|                                                      |            |             |                          |                            |                                                                                                                                                                         |
| ock.mmc.tls.enable                                   | bool       | optional    | false                    | true/false                 | metaservice TLS switch                                                                                                                                                  |
| ock.mmc.tls.ca.path                                  | string     | optional    |                          | 0 <= len < 256             | Root Certificate path                                                                                                                                                   |
| ock.mmc.tls.ca.crl.path                              | string     | optional    |                          | 0 <= len < 256             | Certificate Revocation List path                                                                                                                                        |
| ock.mmc.tls.cert.path                                | string     | optional    |                          | 0 <= len < 256             | server certificate path                                                                                                                                                 |
| ock.mmc.tls.key.path                                 | string     | optional    |                          | 0 <= len < 256             | server private key path                                                                                                                                                 |
| ock.mmc.tls.key.pass.path                            | string     | optional    |                          | 0 <= len < 256             | server private key passphrase path (leave empty if the private key is not encrypted)                                                                                    |
| ock.mmc.tls.package.path                             | string     | optional    |                          | 0 <= len < 256             | openssl dynamic libraries path                                                                                                                                          |
| ock.mmc.tls.decrypter.path                           | string     | optional    |                          | 0 <= len < 256             | the keypass decrypter library path (leave empty if the password is not encrypted)                                                                                       |
|                                                      |            |             |                          |                            |                                                                                                                                                                         |
| ock.mmc.config_store.tls.enable                      | bool       | optional    | false                    | true/false                 | config store TLS switch                                                                                                                                                 |
| ock.mmc.config_store.tls.ca.path                     | string     | optional    |                          | 0 <= len < 256             | Root Certificate path for config store                                                                                                                                  |
| ock.mmc.config_store.tls.ca.crl.path                 | string     | optional    |                          | 0 <= len < 256             | Certificate Revocation List path for config store                                                                                                                       |
| ock.mmc.config_store.tls.cert.path                   | string     | optional    |                          | 0 <= len < 256             | server certificate path for config store                                                                                                                                |
| ock.mmc.config_store.tls.key.path                    | string     | optional    |                          | 0 <= len < 256             | server private key path for config store                                                                                                                                |
| ock.mmc.config_store.tls.key.pass.path               | string     | optional    |                          | 0 <= len < 256             | server private key passphrase path for config store (leave empty if the private key is not encrypted)                                                                   |
| ock.mmc.config_store.tls.package.path                | string     | optional    |                          | 0 <= len < 256             | openssl dynamic libraries path for config store                                                                                                                         |
| ock.mmc.config_store.tls.decrypter.path              | string     | optional    |                          | 0 <= len < 256             | the keypass decrypter library path for config store (leave empty if the password is not encrypted)                                                                      |
|                                                      |            |             |                          |                            |                                                                                                                                                                         |
| ock.mmc.metrics.tls.enable                           | bool       | optional    | false                    | true/false                 | metrics HTTP server mTLS switch                                                                                                                                         |
| ock.mmc.metrics.tls.ca.path                          | string     | optional    |                          | 0 <= len < 256             | Root Certificate path for metrics HTTP                                                                                                                                  |
| ock.mmc.metrics.tls.ca.crl.path                      | string     | optional    |                          | 0 <= len < 256             | Certificate Revocation List path for metrics HTTP                                                                                                                       |
| ock.mmc.metrics.tls.cert.path                        | string     | optional    |                          | 0 <= len < 256             | server certificate path for metrics HTTP                                                                                                                                |
| ock.mmc.metrics.tls.key.path                         | string     | optional    |                          | 0 <= len < 256             | server private key path for metrics HTTP                                                                                                                                |
| ock.mmc.metrics.tls.key.pass.path                    | string     | optional    |                          | 0 <= len < 256             | server private key passphrase path for metrics HTTP (leave empty if the private key is not encrypted)                                                                   |
| ock.mmc.metrics.tls.package.path                     | string     | optional    |                          | 0 <= len < 256             | openssl dynamic libraries path for metrics HTTP                                                                                                                         |
| ock.mmc.metrics.tls.decrypter.path                   | string     | optional    |                          | 0 <= len < 256             | the keypass decrypter library path for metrics HTTP (leave empty if the password is not encrypted)                                                                      |

## LocalService Config

| key                                            | value type | requirement  | default              | valid range                                                                           | description                                                                                                                                                                                                                              |
|------------------------------------------------|------------|--------------|----------------------|---------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| ock.mmc.meta_service_url                       | string     | optional     | tcp://127.0.0.1:5000 | tcp://\<host>\<port>                                                                  | host is cluster-ip/domain in HA, port in [1025, 65535]                                                                                                                                                                                   |
| ock.mmc.local_service.config_store_url         | string     | optional     | tcp://127.0.0.1:6000 | tcp://\<host>\<port>                                                                  | host supports ip and domain, port in [1025, 65535]                                                                                                                                                                                       |
| ock.mmc.log_level                              | string     | optional     | info                 | debug/info/warn/error                                                                 | log level                                                                                                                                                                                                                                |
|                                                |            |              |                      |                                                                                       |                                                                                                                                                                                                                                          |
| ock.mmc.local_service.world_size               | integer    | optional     | 256                  | [1, 1024]                                                                             | The maximum supported rank count; once ranks are connected, no further modifications are allowed — a meta restart is required                                                                                                            |
|                                                |            |              |                      |                                                                                       |                                                                                                                                                                                                                                          |
| ock.mmc.local_service.protocol                 | string     | **required** | host_rdma            | host_rdma/host_urma/host_tcp/host_shm/device_sdma/device_rdma/device_urma/device_uboe | host_shm requires DRAM > 0, HBM = 0, and no hcom.                                                                                                                                                                                        |
| ock.mmc.local_service.hcom_url                 | string     | optional     | tcp://127.0.0.1:7000 | tcp://\<host>\<port>                                                                  | used in dram pool, host supports ip and domain, port in [1024, 65535]                                                                                                                                                                    |
| ock.mmc.local_service.backend_id               | string     | optional     |                      | 0 <= len < 256                                                                        | Backend identity for kv_event (e.g. pod IP); if empty, falls back to env var MMC_LOCAL_SERVICE_BACKEND_ID                                                                                                                                |
|                                                |            |              |                      |                                                                                       |                                                                                                                                                                                                                                          |
| ock.mmc.local_service.dram.size                | integer    | **required** | 1GB                  | [0, max.dram.size]                                                                              | Supports formats such as 134217728, 2048KB, 200mb, 2.5G or 1TB, <br/> and automatic alignment to 2MB (host_rdma, host_tcp or host_shm) or 1GB (device_sdma, device_rdma, device_urma or device_uboe).                                    |
| ock.mmc.local_service.hbm.size                 | integer    | optional     | 0                    | [0, 1TB]                                                                              | Supports formats such as 134217728, 2048KB, 200mb, 2.5G or 1TB, <br/> and automatic alignment to 2MB (host_rdma, host_tcp or host_shm) or 1GB (device_sdma, device_rdma, device_urma or device_uboe). host_shm requires HBM to remain 0. |
| ock.mmc.local_service.max.dram.size            | integer    | optional     | use dram.size        | [dram.size, 16TB]                                                                              | The MAX size of `ock.mmc.local_service.dram.size` in all local processes, necessary if ranks contribute different sizes of DRAM.                                                                                                         |
| ock.mmc.local_service.max.hbm.size             | integer    | optional     | use hbm.size         | [0, 1TB]                                                                              | The MAX size of `ock.mmc.local_service.hbm.size` in all local processes, necessary if ranks contribute different sizes of HBM.                                                                                                           |
| ock.mmc.local_service.storage.enabled          | bool       | optional     | false                | true/false                                                                            | enable SSD storage on local service. When enabled, SSD read/write and data eviction to SSD are supported                                                                                                                                 |
| ock.mmc.local_service.storage.mode | string | optional | standalone | standalone / remote_dram | MemCache lower-tier storage mode; effective with storage.enabled=true. Startup-only; restart LocalService after changes. MemCache reads this setting; UBS-IO reads its own keys from the existing configuration file. |
| ock.mmc.local_service.dram.best_effort.enabled | bool       | optional     | false                | true/false                                                                            | enable best-effort DRAM allocation strategy. When enabled, the system attempts to allocate DRAM                                                                                                                                          |
|                                                |            |              |                      |                                                                                       |                                                                                                                                                                                                                                          |
| ock.mmc.client.retry_milliseconds              | integer    | optional     | 0                    | [0, 600000]                                                                           | The total retry duration (retry interval is 200ms) when client requests meta service and the connection does not exist                                                                                                                   |
| ock.mmc.client.timeout.seconds                 | integer    | optional     | 60                   | [1, 600]                                                                              | Client request timeout in seconds.                                                                                                                                                                                                       |
| ock.mmc.client.read_thread_pool.size           | integer    | optional     | 4                    | [1, 64]                                                                               | Number of threads in the read thread pool.                                                                                                                                                                                               |
| ock.mmc.client.write_thread_pool.size          | integer    | optional     | 4                    | [1, 64]                                                                               | Number of threads in the write thread pool.                                                                                                                                                                                              |
| ock.mmc.client.aggregate.io                    | bool       | optional     | true                 | true/false                                                                            | Whether to enable IO request aggregation for read/write operations.                                                                                                                                                                      |
| ock.mmc.client.aggregate.num                   | integer    | optional     | 122                  | [1, 131072]                                                                           | Number of IO requests to aggregate in a single batch.                                                                                                                                                                                    |
| ock.mmc.client.batch_option.chunk.size         | integer    | optional     | 8MB                  | [0, 1TB]                                                                              | Supports formats such as 134217728, 2048KB, 200mb, 2.5G or 1TB.                                                                                                                                                                          |
| ock.mmc.client.batch_option.chunk.count        | integer    | optional     | 3                    | [1, 64]                                                                               | During batch copy operations, the system will not perform slicing and concurrent copy operations only when the batch count is less than chunk count or the total data size is less than (chunk size * chunk count).                      |
|                                                |            |              |                      |                                                                                       |                                                                                                                                                                                                                                          |
| ock.mmc.tls.enable                             | bool       | optional     | false                | true/false                                                                            | metaservice TLS switch                                                                                                                                                                                                                   |
| ock.mmc.tls.ca.path                            | string     | optional     |                      | 0 <= len < 256                                                                        | Root Certificate path                                                                                                                                                                                                                    |
| ock.mmc.tls.ca.crl.path                        | string     | optional     |                      | 0 <= len < 256                                                                        | Certificate Revocation List path                                                                                                                                                                                                         |
| ock.mmc.tls.cert.path                          | string     | optional     |                      | 0 <= len < 256                                                                        | client certificate path                                                                                                                                                                                                                  |
| ock.mmc.tls.key.path                           | string     | optional     |                      | 0 <= len < 256                                                                        | client private key path                                                                                                                                                                                                                  |
| ock.mmc.tls.key.pass.path                      | string     | optional     |                      | 0 <= len < 256                                                                        | client private key passphrase path (leave empty if the private key is not encrypted)                                                                                                                                                     |
| ock.mmc.tls.package.path                       | string     | optional     |                      | 0 <= len < 256                                                                        | openssl dynamic libraries path                                                                                                                                                                                                           |
| ock.mmc.tls.decrypter.path                     | string     | optional     |                      | 0 <= len < 256                                                                        | the keypass decrypter library path (leave empty if the password is not encrypted)                                                                                                                                                        |
|                                                |            |              |                      |                                                                                       |                                                                                                                                                                                                                                          |
| ock.mmc.config_store.tls.enable                | bool       | optional     | false                | true/false                                                                            | config store TLS switch                                                                                                                                                                                                                  |
| ock.mmc.config_store.tls.ca.path               | string     | optional     |                      | 0 <= len < 256                                                                        | Root Certificate path for config store                                                                                                                                                                                                   |
| ock.mmc.config_store.tls.ca.crl.path           | string     | optional     |                      | 0 <= len < 256                                                                        | Certificate Revocation List path for config store                                                                                                                                                                                        |
| ock.mmc.config_store.tls.cert.path             | string     | optional     |                      | 0 <= len < 256                                                                        | client certificate path for config store                                                                                                                                                                                                 |
| ock.mmc.config_store.tls.key.path              | string     | optional     |                      | 0 <= len < 256                                                                        | client private key path for config store                                                                                                                                                                                                 |
| ock.mmc.config_store.tls.key.pass.path         | string     | optional     |                      | 0 <= len < 256                                                                        | client private key passphrase path for config store (leave empty if the private key is not encrypted)                                                                                                                                    |
| ock.mmc.config_store.tls.package.path          | string     | optional     |                      | 0 <= len < 256                                                                        | openssl dynamic libraries path for config store                                                                                                                                                                                          |
| ock.mmc.config_store.tls.decrypter.path        | string     | optional     |                      | 0 <= len < 256                                                                        | the keypass decrypter library path for config store (leave empty if the password is not encrypted)                                                                                                                                       |
|                                                |            |              |                      |                                                                                       |                                                                                                                                                                                                                                          |
| ock.mmc.local_service.hcom.tls.enable          | bool       | optional     | false                | true/false                                                                            | hcom TLS switch                                                                                                                                                                                                                          |
| ock.mmc.local_service.hcom.tls.ca.path         | string     | optional     |                      | 0 <= len < 256                                                                        | Root Certificate path for hcom                                                                                                                                                                                                           |
| ock.mmc.local_service.hcom.tls.ca.crl.path     | string     | optional     |                      | 0 <= len < 256                                                                        | Certificate Revocation List path for hcom                                                                                                                                                                                                |
| ock.mmc.local_service.hcom.tls.cert.path       | string     | optional     |                      | 0 <= len < 256                                                                        | client certificate path for hcom                                                                                                                                                                                                         |
| ock.mmc.local_service.hcom.tls.key.path        | string     | optional     |                      | 0 <= len < 256                                                                        | client private key path for hcom                                                                                                                                                                                                         |
| ock.mmc.local_service.hcom.tls.key.pass.path   | string     | optional     |                      | 0 <= len < 256                                                                        | client private key passphrase path for hcom (leave empty if the private key is not encrypted)                                                                                                                                            |
| ock.mmc.local_service.hcom.tls.decrypter.path  | string     | optional     |                      | 0 <= len < 256                                                                        | the keypass decrypter library path for hcom (leave empty if the password is not encrypted)                                                                                                                                               |
|                                                |            |              |                      |                                                                                       |                                                                                                                                                                                                                                          |
| ock.mmc.dynamic_config.enable                  | bool       | optional     | false                | true/false                                                                            | Enable dynamic config polling. When enabled, LocalService periodically checks the config file for `meta_service_url` and `config_store_url` changes and lazily updates reconnection targets without restart                              |
| ock.mmc.dynamic_config.interval                | integer    | optional     | 5                    | [1, 300]                                                                              | Polling interval in seconds for dynamic config file check                                                                                                                                                                                |

## KV Events Config

| key                              | value type | requirement | default | valid range          | description                                                               |
|----------------------------------|------------|-------------|---------|----------------------|---------------------------------------------------------------------------|
| ock.mmc.kv_events.enable         | bool       | optional    | false   | true/false           | enable KV event publisher; requires MMC_ENABLE_KV_EVENTS=ON at build time |
| ock.mmc.kv_events.endpoint       | string     | optional    | ""      | tcp://\<host>\<port> | ZMQ PUB bind endpoint, e.g. tcp://0.0.0.0:5557                            |
| ock.mmc.kv_events.model_name     | string     | optional    | ""      | 0 <= len < 256       | model name tag embedded in ZMQ topic and event payload                    |
| ock.mmc.kv_events.tenant_id      | string     | optional    | default | 0 <= len < 256       | tenant identifier in event payload                                        |
| ock.mmc.kv_events.block_size     | integer    | optional    | 0       | [0, 4294967295]      | block size hint in event payload; 0 means not set                         |
| ock.mmc.kv_events.queue_capacity | integer    | optional    | 65536   | [1, 4294967295]      | max number of events in the internal publish queue                        |
| ock.mmc.kv_events.hash_as_int    | bool       | optional    | true    | true/false           | true: serialize seq_hashes as uint64; false: serialize as hex string      |

## Remote DRAM storage configuration

`ock.mmc.local_service.storage.enabled` enables the lower-tier storage path.
`ock.mmc.local_service.storage.mode` selects MemCache behavior: `standalone` (default)
for local SSD, or `remote_dram` for remote DRAM. Unknown values are rejected during
configuration validation. The mode is a startup setting, not a dynamically reloaded option.

Keep the existing configuration file and initialization flow. Add these settings
and the required UBS-IO remote connection parameters to `mmc-local.conf`:

```ini
ock.mmc.local_service.storage.enabled = true
ock.mmc.local_service.storage.mode = remote_dram
ubsio.kvc.mode = remote_dram_tcp
```

LocalService continues to pass `config.configFilePath` through `InitUbsIo` to
`UbsioClientInit`. A nonempty path sets `UBSIO_CONFIG_PATH` internally before SDK
initialization, as before; no new path option or preparation API is required.
MemCache reads its own mode setting, while UBS-IO parses the `ubsio.*` settings
in the same file. Keep the two mode settings consistent. Existing SSD settings
remain in place; remote endpoints, transport, TLS and capacity settings are
interpreted by UBS-IO. Use an absolute configuration path as required by the remote SDK.

In remote mode, LocalService ignores the legacy callback carrying SDK-private chunk
keys and polls the remote eviction journal for translated logical keys, as described
below. MetaService needs no storage-mode setting. `Exists`/`BatchExist` keep the original
metadata check and optional asynchronous
prefetch controlled by `ock.mmc.storage.prefetch.enabled`; they do not wait for a remote
read to verify existence. Actual reads can still fail when a recorded lower-tier copy
is unavailable. `MMC_REMOTE_DRAM_VERIFY_ON_EXISTS` is no longer used, and the build
option `MMC_REMOTE_DRAM_TCP` does not select this runtime behavior.

### Remote DRAM eviction notifications (V2, first stage)

This path is enabled by the existing `storage.enabled=true` and
`storage.mode=remote_dram` settings; it adds no configuration keys or dedicated threads.
Upgrade the BIO server, `libbio_sdk`, UBS-IO KVC library and MemCache together.
Standalone SSD mode can still load libraries without the new polling symbols;
remote mode fails initialization if the symbols or remote journal service are unavailable.

The notification path is:

```text
BIO memory discard succeeds
  -> WCacheManager eviction callback appends a physical key to its ring
  -> BioPollEvictEvents reads a batch using each SDK process's per-node cursor
  -> UbsioKvCachePollMetaEvents filters generations and translates logical keys
  -> LocalService HandleUbsIoMetaEvents removes the local lower-tier descriptor
  -> existing UbsIoMetaDeleteRequest / MetaService RemoveSsdBlob(key, rank)
```

Remote DRAM continues to use the existing SSD/lower-tier descriptor type. The last
step removes that rank's lower-tier descriptor while retaining DRAM/HBM and other-rank
copies. The request format and MetaService handler are unchanged.

**BIO journal.** `WCacheManager` allocates a ring only when `hasDiskCache=false`.
Each record contains a monotonically increasing `seq` and the physical key; the
node also keeps a random nonzero `epoch` for this initialization and `nextSeq`.
The ring has 65,536 records. With a 256-byte key field and 8-byte sequence number,
its record storage is 16.5 MiB per node, independent of DRAM capacity. Appending
uses a short mutex-protected overwrite and performs no RPC. Reads copy at most
64 records while holding the same mutex; callbacks run after it has been released.

There is no acknowledgement table or per-client queue on the server. Reading does
not delete records: the oldest slot is overwritten as the ring advances. Multiple
SDK processes can therefore consume the same node log independently. The protocol
returns `epoch`, `firstSeq` (oldest retained sequence), `nextSeq`, status and records.
A cursor before `firstSeq-1`, or beyond the current tail, receives `GAP`; a changed
epoch receives `EPOCH_CHANGED`.

**SDK cursors.** The only new BIO opcode is `BIO_OP_SDK_READ_EVICT_LOG`, appended to
the opcode enum to preserve existing values. `BioPollEvictEvents(callback, context,
limit)` uses the existing node view, configured remote IPs and synchronous control
channel. One SDK process supports one polling consumer. During LocalService startup,
`limit=0` records every configured node's current tail before backup writers are
enabled. It does not replay historical events or reset an established cursor.
Positive limits must be at most 64 and require successful bootstrap.

Each successful node batch is delivered synchronously before its cursor advances.
A transport failure retains that node's cursor for the next attempt. Other nodes
may have delivered batches even when the overall return code is nonzero. A gap,
epoch change, or previously unseen node after bootstrap latches a reconciliation
requirement; subsequent calls do not silently jump to a new tail. `limit=0` cannot
clear this state. Reconciliation of lower-tier descriptors must happen externally
before SDK exit/reinitialization; merely restarting the poll is not reconciliation.

**Logical key translation.** `UbsioKvCachePollMetaEvents` reuses the existing C event
type and the KVC process's `committed` directory. It validates the physical key
format, finds the existing hash shard, and matches key hash, generation and chunk
index (including the manifest sentinel). Events for older generations or objects
absent from that directory are ignored. A chunk or manifest eviction invalidates
the whole logical object's lower-tier copy. Duplicate logical key/generation pairs
are merged within one polling call. Directory locks are released before invoking
MemCache; the directory and payload are not deleted by this notification path.
Translation visits one existing directory shard per physical record (approximately
`logical_key_count / 256` entries with balanced hashing); it performs no per-chunk
existence RPC and adds no reverse-index maintenance to Put/Delete.

**Scheduling and shutdown.** LocalService registers a one-second task with the
existing periodic scheduler. The timer only submits work to `ubsioEventPool_`.
A stored future prevents overlapping polls, so a slow RPC does not create a growing
queue of polling tasks. The worker consumes at most one batch per node and calls
the existing metadata-delete handler directly. Shutdown first unregisters the timer,
then waits for the in-flight poll and its synchronous callbacks, then tears down
callbacks, the pool and UBS-IO. No polling thread is added in BIO or KVC.

**First-stage limits.** Notifications are asynchronous: a Get can still race with
eviction before metadata is updated. The existing void callback does not acknowledge
MetaService delivery; this version adds neither durable retries for a failed metadata
RPC nor an automatic full scan after GAP. Generation filtering is local to KVC:
the unchanged MetaService request carries key/rank only, so it does not provide
end-to-end version protection against a concurrent replacement/publication race.
These require a subsequent design change before claiming reliable invalidation.

The initial polling budget is 64 physical records per node per second at best,
less when the shared pool or RPCs are slow. Idle responses are fixed-size (16,928
bytes with the current layout). If the journal arrival rate exceeds consumption,
backlog grows and eventually causes GAP. For a stopped consumer the retention window
is approximately `65536 / eviction_records_per_second`; tuning the batch/drain budget
requires load testing. This first version has no automatic recovery from sustained
overload. Poll failures and recovery are logged on status changes; journal GAP/epoch
details are logged by BIO SDK when detected.
