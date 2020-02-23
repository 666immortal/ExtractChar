#### 文件夹介绍

- material文件夹 - 存放原材料
  segmentChar文件夹 - 
  		    staticTailor - 单张图片从ROI中提取每一个字符块并排序
  		    getBlockfromMovie - 从视频（patient.avi）中提取ROI中的每一个字符块并排序、筛选
- trainSVM文件夹 - 训练支持向量机（使用从视频中提取发的字符块），numchar_svm.xml为训练好的存储文件
- applySVM文件夹 - 使用训练好的支持向量机来识别视频中的数字



#### 运行结果

![提取文本](D:\ExtractChar\cut.JPG)



