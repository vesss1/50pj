# 影像處理專案報告 - 區域放大與畫筆功能

## 專案概述
本專案為基於 Qt5 框架開發的影像處理應用程式，主要實現了以下功能：
1. 在主視窗中透過拖移滑鼠選取區域的方式放大照片
2. 自訂放大倍率
3. 在新視窗中顯示放大後的影像
4. 提供畫筆功能以便在放大影像上進行標註
5. 提供另存新檔功能以儲存編輯後的影像

## 使用的指令與開發流程

### 1. 環境準備
首先需要安裝 Qt5 開發環境：
```bash
sudo apt update
sudo apt install -y qt5-qmake qtbase5-dev qtbase5-dev-tools
```

**說明**：
- `qt5-qmake`：Qt5 的建置工具，用於產生 Makefile
- `qtbase5-dev`：Qt5 基本開發套件，包含核心功能模組
- `qtbase5-dev-tools`：Qt5 開發工具，包含 moc（Meta-Object Compiler）等

### 2. 專案建置
```bash
cd /home/runner/work/50pj/50pj
qmake ImageProcessor.pro
make
```

**說明**：
- `qmake ImageProcessor.pro`：讀取專案檔案並產生對應的 Makefile
- `make`：根據 Makefile 編譯所有原始碼並連結成執行檔

### 3. 執行程式
```bash
./ImageProcessor
```

## 實作內容解讀

### 核心檔案架構

#### 1. zoomwindow.h / zoomwindow.cpp
**目的**：建立新的放大視窗類別，提供影像顯示、繪圖與儲存功能。

**主要功能**：
- **影像顯示**：使用 `QLabel` 和 `QScrollArea` 顯示放大後的影像
- **畫筆功能**：
  - 透過 `mousePressEvent`、`mouseMoveEvent` 和 `mouseReleaseEvent` 捕捉滑鼠事件
  - 使用 `QPainter` 在影像上繪製線條
  - 可調整筆刷顏色（透過 `QColorDialog`）
  - 可調整筆刷寬度（透過 `QSpinBox`）
- **另存新檔**：使用 `QFileDialog::getSaveFileName` 提供檔案儲存對話框

**關鍵程式碼片段**：
```cpp
void ZoomWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isDrawingMode && drawing && (event->buttons() & Qt::LeftButton)) {
        QPoint labelPos = imageLabel->mapFrom(this, event->pos());
        if (imageLabel->rect().contains(labelPos)) {
            QPainter painter(&drawingImage);
            painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, 
                               Qt::RoundCap, Qt::RoundJoin));
            painter.drawLine(lastPoint, labelPos);
            lastPoint = labelPos;
            imageLabel->setPixmap(QPixmap::fromImage(drawingImage));
        }
    }
}
```

#### 2. imageprocessor.h / imageprocessor.cpp 的修改
**目的**：在主視窗中新增區域選取與放大功能。

**主要修改**：
- **新增標頭檔引用**：
  - `<QRubberBand>`：用於顯示選取框
  - `<QInputDialog>`：用於輸入放大倍率
  - `"zoomwindow.h"`：放大視窗類別
  
- **新增成員變數**：
  - `QRubberBand *rubberBand`：選取框物件
  - `QPoint origin`：記錄滑鼠按下的起始點
  - `bool selectingRegion`：標記是否處於區域選取模式
  
- **新增功能按鈕**：
  - `regionZoomAction`：區域放大功能的 Action，加入工具列和選單

**區域選取流程**：
1. 使用者點擊「區域放大」按鈕，啟動選取模式
2. 在影像上按下滑鼠左鍵，記錄起始點
3. 拖曳滑鼠時，`QRubberBand` 會即時顯示選取範圍
4. 放開滑鼠後，彈出對話框要求輸入放大倍率
5. 計算選取區域對應的影像座標並進行放大
6. 開啟新的 `ZoomWindow` 顯示放大後的影像

**關鍵程式碼片段**：
```cpp
void ImageProcessor::mouseReleaseEvent(QMouseEvent *event)
{
    if (selectingRegion && event->button() == Qt::LeftButton && rubberBand) {
        // 取得選取範圍
        QRect selectedRect = rubberBand->geometry();
        
        // 轉換座標系統（視窗座標 -> 影像標籤座標 -> 實際影像座標）
        QPoint labelTopLeft = imgWin->mapFrom(this, selectedRect.topLeft());
        QPoint labelBottomRight = imgWin->mapFrom(this, selectedRect.bottomRight());
        QRect labelRect(labelTopLeft, labelBottomRight);
        
        // 計算實際影像座標（考慮縮放比例）
        double scaleX = (double)img.width() / imgWin->width();
        double scaleY = (double)img.height() / imgWin->height();
        
        int imgX = qMax(0, (int)(labelRect.x() * scaleX));
        int imgY = qMax(0, (int)(labelRect.y() * scaleY));
        int imgW = qMin(img.width() - imgX, (int)(labelRect.width() * scaleX));
        int imgH = qMin(img.height() - imgY, (int)(labelRect.height() * scaleY));
        
        // 要求輸入放大倍率
        bool ok;
        double zoomRatio = QInputDialog::getDouble(this, tr("放大倍率"),
            tr("請輸入放大倍率:"), 2.0, 0.1, 10.0, 1, &ok);
        
        if (ok) {
            // 提取並放大選取區域
            QImage selectedRegion = img.copy(imgX, imgY, imgW, imgH);
            QImage zoomedImage = selectedRegion.scaled(
                selectedRegion.width() * zoomRatio,
                selectedRegion.height() * zoomRatio,
                Qt::KeepAspectRatio, Qt::SmoothTransformation);
            
            // 開啟放大視窗
            openZoomWindow(zoomedImage);
        }
    }
}
```

#### 3. ImageProcessor.pro 的修改
新增了兩個檔案到專案中：
```
SOURCES += zoomwindow.cpp
HEADERS += zoomwindow.h
```

## 執行結果

### 編譯結果
專案成功編譯，產生可執行檔 `ImageProcessor`。編譯過程包括：
1. 編譯所有 `.cpp` 檔案為目標檔 (`.o`)
2. 使用 Qt 的 moc 工具處理含有 `Q_OBJECT` 巨集的標頭檔
3. 連結所有目標檔與 Qt 函式庫（QtWidgets、QtGui、QtCore）

### 功能驗證

#### 1. 區域選取功能
- 點擊工具列的「區域放大」按鈕
- 滑鼠游標變更為十字形狀
- 拖曳滑鼠可看到虛線選取框（QRubberBand）
- 選取框會即時跟隨滑鼠移動

#### 2. 放大倍率設定
- 放開滑鼠後，彈出對話框
- 預設倍率為 2.0
- 可調整範圍：0.1 ~ 10.0 倍
- 支援小數點後一位

#### 3. 放大視窗功能
- 成功開啟新視窗顯示放大後的影像
- 視窗包含工具列，提供以下功能：
  - **另存新檔**：可選擇 PNG、JPEG、BMP 格式
  - **畫筆模式**：可切換的按鈕（Checkable）
  - **選擇顏色**：彈出色彩選擇對話框
  - **筆寬調整**：使用 SpinBox 調整 1-50 像素

#### 4. 畫筆功能
- 啟用畫筆模式後，游標變更為十字形狀
- 按住滑鼠左鍵拖曳可在影像上繪製線條
- 線條顏色和寬度可即時調整
- 繪製的內容會直接修改影像資料

#### 5. 儲存功能
- 點擊「另存新檔」開啟檔案儲存對話框
- 儲存的影像包含所有繪製的內容
- 成功儲存後會顯示確認訊息

## AI 協作理解與心得

### 1. 需求分析與架構設計
在與 AI 協作的過程中，首先要明確表達需求。本專案的需求包含多個功能點：
- **區域選取**：需要視覺化的選取框
- **座標轉換**：從視窗座標到影像座標的映射
- **獨立視窗**：放大影像需要在新視窗中顯示
- **繪圖功能**：需要事件處理和圖形繪製

AI 能夠理解這些需求並提出合適的 Qt 元件解決方案（QRubberBand、QPainter 等）。

### 2. 程式碼組織與模組化
AI 建議將放大視窗功能獨立為一個新類別（ZoomWindow），這是良好的軟體工程實踐：
- **單一職責原則**：每個類別只負責一項功能
- **可重用性**：ZoomWindow 可以被其他功能重複使用
- **維護性**：修改放大視窗功能不會影響主視窗

### 3. 座標系統的處理
影像處理中最容易出錯的是座標轉換。本專案涉及三層座標系統：
1. **視窗座標**：滑鼠事件的原始座標
2. **標籤座標**：QLabel（imgWin）內的座標
3. **影像座標**：實際影像資料的像素座標

AI 協助正確處理了這些轉換，避免了邊界錯誤。

### 4. Qt 事件機制的應用
AI 展示了如何正確使用 Qt 的事件處理機制：
- **事件重載**：重載 `mousePressEvent` 等方法
- **事件傳遞**：在不同模式下選擇性處理或傳遞事件
- **即時回饋**：透過 `mouseMoveEvent` 提供即時的視覺回饋

### 5. 使用者體驗設計
AI 在實作中考慮了使用者體驗：
- **游標變更**：在不同模式下使用不同游標（十字、箭頭）
- **狀態列訊息**：即時顯示操作提示
- **預設值設定**：放大倍率預設 2.0，筆寬預設 3
- **視窗屬性**：使用 `Qt::WA_DeleteOnClose` 自動管理記憶體

### 6. 錯誤處理與邊界檢查
程式碼中包含多處邊界檢查：
- 確認選取範圍大小 (`width > 10 && height > 10`)
- 確認座標在影像範圍內 (`qMax`, `qMin`)
- 確認影像已載入 (`!img.isNull()`)
- 檢查檔案儲存是否成功

### 7. 漸進式開發
與 AI 協作時採用了漸進式開發方法：
1. 先建立基本的視窗類別
2. 再加入區域選取功能
3. 然後實作放大邏輯
4. 最後加入繪圖和儲存功能

這種方法便於測試和除錯，每一步都能確保功能正常。

## 技術重點總結

### Qt 元件的選擇
- **QRubberBand**：提供視覺化的選取框，是 Qt 中標準的區域選取解決方案
- **QInputDialog**：快速取得使用者輸入，無需建立完整對話框
- **QScrollArea**：處理大型影像的顯示，自動提供捲軸
- **QPainter**：強大的 2D 繪圖 API，支援各種繪圖操作

### 座標系統處理
本專案涉及複雜的座標轉換：
- **主視窗座標轉換**：從視窗座標 → QLabel 座標 → 影像像素座標
  - 主視窗使用 `setScaledContents(true)`，影像會縮放以符合 QLabel 大小
  - 需要計算縮放比例來正確對應影像像素
- **放大視窗座標**：放大視窗中的影像以實際尺寸顯示（無縮放）
  - 使用 `adjustSize()` 讓 QLabel 符合影像大小
  - 放在 QScrollArea 中處理超出視窗的部分
  - 繪圖座標直接對應影像像素，無需轉換

### 設計模式的應用
- **觀察者模式**：Qt 的信號與槽機制（Signal/Slot）
- **模板方法模式**：事件處理函數的重載（Override）
- **策略模式**：根據 `isDrawingMode` 切換不同的滑鼠行為

### 程式碼品質
- 清晰的變數命名（origin、rubberBand、selectingRegion）
- 適當的註解與分段
- 一致的程式碼風格（符合 Qt 慣例）
- 記憶體管理（使用 Qt 的父子物件機制）

## 結論

本專案成功實現了影像區域放大與標註功能，展示了 Qt 框架在桌面應用程式開發的強大能力。透過與 AI 的協作，能夠快速理解需求、選擇合適的技術方案、實作功能並進行優化。

AI 協作的優勢在於：
1. **快速原型開發**：能迅速產生可運行的程式碼
2. **技術選型建議**：推薦適合的函式庫和元件
3. **最佳實踐指導**：遵循軟體工程原則
4. **問題診斷**：協助發現和修正錯誤（如 QPainter 的拼寫錯誤）

然而，開發者仍需要：
1. **明確表達需求**：清楚說明想要的功能
2. **理解程式碼邏輯**：不能只是複製貼上，要真正理解實作原理
3. **測試與驗證**：確保功能符合預期
4. **持續學習**：理解 AI 使用的技術和設計模式，提升自身能力

這種人機協作的開發模式將是未來軟體開發的重要趨勢。
