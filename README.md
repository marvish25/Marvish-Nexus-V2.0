# 🌌 Marvish Nexus V2.0
> **The Neural Link Between Local Systems and Global Accessibility.**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Language: C++](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)](https://isocpp.org/)
[![Network: Cloudflare](https://img.shields.io/badge/Network-Cloudflare_Tunnel-orange.svg)](https://www.cloudflare.com/products/tunnel/)
[![Backend: Crow](https://img.shields.io/badge/Framework-Crow_v1.0-brightgreen.svg)](https://crowcpp.org/)

Marvish Nexus V2.0 is a high-performance, full-stack ecosystem designed to bridge the gap between low-level system architecture and modern web accessibility. By leveraging a high-concurrency **C++ backend** (Crow framework) and **Cloudflare Zero Trust** tunneling, it allows local hardware to serve global requests with enterprise-grade security. The system integrates a **PostgreSQL** relational database for multi-tenant management and a local **TinyLlama** neural interface to provide real-time, AI-driven system diagnostics and interaction.

---

## 🛠 High-Level Architecture

Nexus V2.0 isn't just a website; it's a distributed infrastructure experiment.

* **Core Engine:** Written in **C++** using the Crow framework for ultra-low latency request handling.
* **Security Layer:** Integrated **Cloudflare Zero Trust** (Argo Tunnel) to expose local services without port forwarding.
* **Neural Link:** Localized **TinyLlama 1.1B** inference engine for natural language system commands.
* **Data Persistence:** Scalable **PostgreSQL** instance managing relational user data and system state.
* **Frontend:** A reactive, glassmorphic dashboard designed for real-time system monitoring.

---

## ✨ Key Features

* **🔒 Secure Edge Tunneling:** Bypasses CGNAT and local firewalls using `cloudflared`, allowing the Dell OptiPlex 390 node to be accessible from any network (e.g., University of Limpopo).
* **🧠 Local AI Integration:** A self-hosted LLM interface that interprets system logs and manages user-defined "Jobs" without data leaving the local node.
* **⚡ High-Performance Backend:** Utilizing C++'s memory management and multi-threading to handle authentication and API routing.
* **📊 System Command Center:** A centralized dashboard for user management, resource monitoring, and remote command execution.
* **🛡 Zero-Exposure Networking:** No open inbound ports—all traffic is routed through encrypted outbound tunnels.

---

## 🚀 Technical Stack

| Component | Technology |
| :--- | :--- |
| **Language** | C++17 / C++20 |
| **Backend Framework** | Crow (Micro-framework) |
| **Intelligence** | TinyLlama (via llama.cpp/local inference) |
| **Database** | PostgreSQL |
| **Networking** | Cloudflare Tunneling, WireGuard |
| **Environment** | Linux Ubuntu (Production) |

---

## 🔨 Installation & Setup

1. **Clone the Nexus:**
   ```bash
   git clone [https://github.com/Marvish25/marvish-nexus-v2.git](https://github.com/Marvish25/marvish-nexus-v2.git)
   cd marvish-nexus-v2
