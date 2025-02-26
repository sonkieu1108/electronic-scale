# Cân Điện Tử Sử Dụng STM32 và Loadcell và HX711

Dự án này là code cho cân điện tử sử dụng vi điều khiển STM32 NUCLE0 F401RE, cảm biến tải HX711, LOADCELL và LCD I2C.
Các Thư Viện
liquidcrystal_i2c.h và liquidcrystal_i2c.c: Thư viện cho LCD I2C
1. Hiệu chỉnh hệ số chuyển đổi (knownOriginal và knownHX711)
Chuẩn bị khối lượng tham chiếu:
Sử dụng quả cân mẫu chuẩn hoặc một vật có khối lượng chính xác đã biết (ví dụ: 50g, 100g).
Đảm bảo khối lượng tham chiếu có độ chính xác cao.
Đặt khối lượng lên cân:
Đặt khối lượng tham chiếu lên bàn cân.
Đảm bảo khối lượng được đặt ổn định và không bị rung lắc.
Ghi lại giá trị ADC:
Mở serial monitor hoặc debug console trong STM32CubeIDE.
Quan sát và ghi lại giá trị ADC từ HX711.
Giá trị ADC này sẽ được sử dụng để tính toán hệ số chuyển đổi.
Cập nhật code:
Mở file main.c trong dự án STM32 của bạn.
Tìm đến các dòng định nghĩa knownOriginal và knownHX711.
Thay thế knownOriginal bằng khối lượng tham chiếu (ví dụ: 50).
Thay thế knownHX711 bằng giá trị ADC đã ghi lại.
Biên dịch và nạp lại code:
Biên dịch lại dự án STM32.
Nạp code đã cập nhật lên STM32.
Kiểm tra lại giá trị khối lượng trên LCD để đảm bảo độ chính xác.
2. Hiệu chỉnh điểm 0 (tare)
Đảm bảo không có tải trên cân:
Lấy tất cả các vật ra khỏi bàn cân.
Đảm bảo bàn cân hoàn toàn trống rỗng.
Ghi lại giá trị ADC:
Mở serial monitor hoặc debug console.
Quan sát và ghi lại giá trị ADC từ HX711 khi không có tải.
Giá trị ADC này sẽ được sử dụng để thiết lập điểm 0.
Cập nhật code:
Mở file main.c trong dự án STM32 của bạn.
Tìm đến dòng định nghĩa tare.
Thay thế tare bằng giá trị ADC đã ghi lại.
Biên dịch và nạp lại code:
Biên dịch lại dự án STM32.
Nạp code đã cập nhật lên STM32.
Kiểm tra lại giá trị khối lượng trên LCD, đảm bảo hiển thị 0 khi không có tải.
3. Lọc nhiễu (NOISE_MARGIN)
Quan sát giá trị khối lượng trên LCD:
Quan sát giá trị khối lượng hiển thị trên LCD khi không có tải.
Nếu thấy giá trị dao động không ổn định, cần lọc nhiễu.
Điều chỉnh NOISE_MARGIN:
Mở file main.c trong dự án STM32 của bạn.
Tìm đến dòng định nghĩa NOISE_MARGIN.
Tăng giá trị NOISE_MARGIN để lọc bỏ các dao động nhỏ.
Giá trị NOISE_MARGIN càng lớn, khả năng lọc nhiễu càng cao, nhưng độ nhạy có thể giảm.
Kiểm tra và điều chỉnh:
Thử nghiệm với các giá trị NOISE_MARGIN khác nhau (ví dụ: 100, 200, 500).
Quan sát giá trị khối lượng trên LCD để tìm giá trị phù hợp.
Cân bằng giữa khả năng lọc nhiễu và độ nhạy của cân.
Biên dịch và nạp lại code:
Biên dịch lại dự án STM32.
Nạp code đã cập nhật lên STM32.
Kiểm tra lại giá trị khối lượng trên LCD để đảm bảo ổn định.
Lưu Ý
Đảm bảo nguồn điện cung cấp cho HX711 và LCD phù hợp.
Kiểm tra kỹ các kết nối dây để tránh sai sót.
Điều chỉnh các giá trị hiệu chỉnh để đạt được độ chính xác mong muốn.
Nếu cân hiển thị giá trị âm khi không có tải, cần kiểm tra lại giá trị tare.
Nếu giá trị khối lượng không ổn định, hãy kiểm tra kết nối dây và nguồn điện.


