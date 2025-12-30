# ImageProcessor

Qt5 影像處理應用程式，支援區域選取放大、繪圖標註等功能。

## 功能特色

### 基本功能
- 開啟與顯示影像檔案（BMP、PNG、JPEG）
- 影像幾何轉換（鏡射、旋轉）
- 滑鼠事件追蹤與座標顯示

### 進階功能（新增）
- **區域選取放大**：拖曳滑鼠選取影像區域，自訂放大倍率
- **繪圖標註**：在放大影像上使用畫筆進行標註
- **彈性筆刷**：可調整筆刷顏色與粗細
- **另存新檔**：儲存編輯後的影像

## 建置說明

### 環境需求
- Qt 5.x
- C++17 或更高版本
- qmake 建置工具

### 編譯步驟
```bash
# 安裝 Qt5 開發套件（Ubuntu/Debian）
sudo apt install qt5-qmake qtbase5-dev qtbase5-dev-tools

# 產生 Makefile
qmake ImageProcessor.pro

# 編譯
make

# 執行
./ImageProcessor
```

## 使用說明

詳細的使用說明與操作流程，請參閱 [FEATURES_SUMMARY.md](FEATURES_SUMMARY.md)

完整的技術文件與實作說明，請參閱 [PROJECT_REPORT.md](PROJECT_REPORT.md)

## 授權

本專案為學習與研究用途。