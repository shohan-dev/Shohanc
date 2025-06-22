## 📚 Modular Components

The `shohanc` library is built as a collection of production-ready, high-performance components designed for use in data-intensive and backend systems. Every module is cleanly separated under `shohanc.collections` and can be used independently or together.

### 🔁 UltraQueue — High-Performance Hybrid Queue
A blazing-fast, enterprise-grade RAM + disk queue system with optional native C backend support. Supports persistence, AES encryption, multiprocessing/threading, auto save, and crash recovery.

#### ✅ Features
- Dual backend: Python deque or C-based ultra queue
- Auto-save with compression and encryption
- Thread-safe & process-safe
- Instant batch operations
- Real-world tested with million+ ops/sec throughput

🔗 [Read Full Docs →](https://github.com/shohan-dev/Shohanc/blob/main/docs/ultraqueue.md)

#### 📦 Import:
```python
from shohanc.collections import UltraQueue
```

#### 🧪 Example:
```python
queue = UltraQueue(save_path="queue.dat", encryption_key="secret", use_ultraqueue=True)
queue.push("data")
print(queue.pop())
```

### 🧠 JSONUtils — Smart JSON Operations *(Coming Soon)*
A powerful toolkit for smart JSON processing in backend and data systems. Will include:

- Deep comparison (diff, patch)
- JSON schema validation and enforcement
- Auto flattening, restructuring, key renaming
- Compact serialization, pretty printing, transformation chaining

🔗 [Planned Documentation →](https://github.com/shohan-dev/Shohanc/blob/main/docs/jsonutils.md)

#### 📦 Future:
```python
from shohanc.collections import JSONUtils
```

### 💾 DataStore — Persistent Key-Value Store *(Planned)*
Crash-safe lightweight key-value database with:

- Atomic writes
- Auto backup and snapshot
- Read/write optimized design
- Built-in Fernet encryption

🔗 [Planned Documentation →](https://github.com/shohan-dev/Shohanc/blob/main/docs/datastore.md)

#### 📦 Future:
```python
from shohanc.collections import DataStore
```

### 📖 More Modules Coming Soon...

| Module       | Description                                      | Status       |
|--------------|--------------------------------------------------|--------------|
| 🧬 BlobStore | Binary-safe object store with encryption & versioning | ⏳ Planned   |
| 🧮 MathUtils | Advanced numerical and matrix utilities          | ⏳ Planned   |
| 🧪 TestUtils | Built-in stubs and mocks for queue/data testing  | ✅ Internal  |
| 🔍 SearchKit | Index-based fast filtering & text search utilities | ⏳ Research  |

### 🌐 Why Modular?

| Benefit            | Description                                      |
|--------------------|--------------------------------------------------|
| 🔌 Plug & Play     | Only use what you need — import minimal components |
| 🧩 Extensible Design | Easily build your own modules on top of ours     |
| 🛠️ Future Ready    | Built to support web servers, AI pipelines, and microservices |
| 📦 Real-World Proven | Optimized and tested with heavy data + production usage |

### 📚 Documentation & Usage

Each module comes with:

- 🧠 Conceptual overview
- 🧪 Code examples & tips
- 💻 Real-world scenarios
- ⚠️ Pitfalls to avoid
- 🔐 Best practices (security, performance)

To explore individual modules, visit the relevant links:

- [UltraQueue Documentation](https://github.com/shohan-dev/Shohanc/blob/main/docs/ultraqueue.md)
- [JSONUtils Documentation (soon)](https://github.com/shohan-dev/Shohanc/blob/main/docs/jsonutils.md)
- [DataStore Documentation (planned)](https://github.com/shohan-dev/Shohanc/blob/main/docs/datastore.md)

### 🚀 Contributing New Modules?

If you'd like to add a new component or module under `shohanc.collections`, check the `CONTRIBUTING.md` guide.

---