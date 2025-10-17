## Overview
DyMAC is a low-power, long-range **Dynamic MAC protocol** designed for Industrial IoT (IIoT) networks. 
It implements the IEEE 802.15.4e TSCH standard on **ARM Cortex M4 (nRF52840)** devices, enabling reliable and scalable wireless communication
This project is **MeitY-funded** and executed in collaboration with **IIT Hyderabad, IIT Bhubaneswar, IISc Bangalore, and CDAC Bangalore**.  

The Dynamic MAC provides:  
- **TDMA slotframe structure** for determinism and scalability  
- **FDMA structure** to mitigate interference  
- Multi-hop synchronization and **PAN formation**  
- Clear Channel Assessment using energy thresholding  
- Frequency and channel hopping for reliable communication  
- Frame structures for **Enhanced Beacon, Data Frame, and Enhanced Acknowledgment**  

PLatform:  
**Embedded Platform:** Dynamic MAC implementation on ARM Cortex M4 microcontroller

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
- **Hardware:** ARM Cortex M4 (nRF52840), Radios - ADF7242 for 2.4 GHz and ADF7030-1 for 863 MHz 
- **Protocols:** IEEE 802.15.4e-TSCH  
- **Tools / Platforms:** SEGGER Embedded Studio, ns-3, Oscilloscope, Logic Analyzer, FreeRTOS  

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

