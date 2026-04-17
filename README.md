__Description of System__

This project focuses on developing a Virtual Child Safety Fence using wireless communication and proximity detection. The system creates an invisible boundary around a child, ensuring real-time monitoring and alerts when the child crosses predefined safety zones.The system utilizes BLE (Bluetooth Low Energy) modules (HM-10) and signal strength (RSSI) analysis to determine distance. Based on proximity levels, alerts are triggered to notify guardians. The implementation emphasizes low power consumption, portability, and real-time response.

__Implementation of System__\
__<#>__ Initially, the system was designed using a basic proximity detection model between two BLE devices (parent and child nodes).\
__<#>__ The child node continuously broadcasts signals, while the parent node scans and filters using UUID-based identification.\
__<#>__ A multi-level RSSI boundary model (4 safety zones) is implemented to classify distance ranges accurately.\
__<#>__ To improve stability, signal smoothing and persistence counters are used to avoid false triggers due to signal fluctuations.\
__<#>__ Alerts are triggered only during state transitions (zone changes) to prevent repeated notifications.\
__<#>__ The system supports real-time alert mechanisms such as buzzer notifications and mobile alerts (via WiFi integration like Blynk).

__Safety Zones of System__\
__<$>__ Zone 1 (Safe Zone): Child is within close proximity – no alerts required.\
__<$>__ Zone 2 (Warning Zone): Slight increase in distance – mild alert or monitoring state.\
__<$>__ Zone 3 (Risk Zone): Child moving away – stronger alert triggered.\
__<$>__ Zone 4 (Critical Zone): Child out of safe boundary – immediate high-priority alert activated.\
__<$>__ The zoning system ensures graded response instead of abrupt triggering, improving reliability.

__Advantages of System__\
__<$>__ Provides real-time child monitoring without physical barriers.\
__<$>__ Reduces risk of children wandering into unsafe areas.\
__<$>__ Low-cost and energy-efficient implementation using BLE.\
__<$>__ Eliminates need for GPS (works effectively in indoor environments).\
__<$>__ Scalable for multiple children or extended monitoring systems.

__Future Enhancements__\
__<%>__ Priority handling can be extended for emergency scenarios, such as detecting sudden signal loss or abnormal movement patterns.\
__<%>__ Integration with AI-based behavior analysis can predict unsafe movement trends.\
__<%>__ System can be expanded with IoT cloud connectivity for remote monitoring and data logging.\
__<%>__ Future upgrades may include wearable integration and improved accuracy using hybrid sensing (BLE + UWB).\
__<%>__ The system can be adapted for elderly monitoring, asset tracking, and industrial safety applications.
