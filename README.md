# DyMAC-TSCH
Low-power, long-range Dynamic MAC protocol implementation for IIoT networks (IEEE 802.15.4e TSCH) on nrf52840 (ARM Cortex M4)
# DyMAC – Dynamic MAC Protocol (IEEE 802.15.4e TSCH)

## Overview
**DyMAC** is a **Dynamic MAC Protocol** developed for **low-power, long-range radios** in the **Industrial Internet of Things (IIoT)**.  
This project is **MeitY-funded** and executed in collaboration with **IIT Hyderabad, IIT Bhubaneswar, IISc Bangalore, and CDAC Bangalore**.  

The **IEEE 802.15.4e TSCH standard–based MAC** implemented in DyMAC provides:  
- **TDMA slotframe structure** for determinism and scalability  
- **FDMA structure** to mitigate interference  
- Multi-hop synchronization and **PAN formation**  
- Clear Channel Assessment using energy thresholding  
- Frequency and channel hopping for reliable communication  
- Frame structures for **Enhanced Beacon, Data Frame, and Enhanced Acknowledgment**  

The project has **two main platforms**:  
1. **GNU Radio:** IEEE 802.15.4e TSCH MAC Out-of-Tree (OOT) module  
2. **Embedded Platform:** Dynamic MAC implementation on **ARM Cortex M4 microcontroller**

---

## My Role / Contributions
- Developed **Dynamic MAC on ARM M4 microcontroller**  
- Designed **TDMA/FDMA-based MAC API** for enhanced reliability and scalability  
- Implemented **multi-hop synchronization** and PAN formation  
- Tested and optimized **low-power, long-range performance** in real hardware  
- Contributed to **documentation and research publication**

---

## Tech Stack
- **Languages:** C, Embedded C  
- **Hardware:** ARM Cortex M4 (nRF52840), Industrial IoT radios  
- **Protocols:** IEEE 802.15.4e, TSCH  
- **Tools / Platforms:** SEGGER Embedded Studio, GNU Radio, ns-3, Oscilloscope, Logic Analyzer, FreeRTOS  

---

## Paper & Resources
- 📄 Published Paper: [Dy-MAC: Implementation of Dynamic MAC Stack for IEEE 802.15.4e TSCH](https://cni.iisc.ac.in/assets/pdf/papers/Dy-MAC_Implementation_of_dynamic_MAC_stack_for_IEEE_802.15.4e-TSCH.pdf)  
- 🌐 Project Website / Demo: *(add link if available)*  

---

## Network Architecture
<p align="center">
  <img src="images/network_topology.png" alt="Network Topology" width="600"/>
</p>

**Figure:** Multi-hop industrial IoT network topology used for DyMAC testing.  

---

## Multi-Hop Synchronization Example
<p align="center">
  <img src="images/mesh_sync_example.png" alt="Mesh Synchronization" width="600"/>
</p>

**Figure:** Multi-hop TDMA slot synchronization in real-time embedded deployment.  

---

## Repository Structure

