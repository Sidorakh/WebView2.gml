@echo off
copy "..\x64\Release\WebView2.gml.dll" "..\GM\WebView2.gml\extensions\WebView2\"
copy "..\x64\Release\WebView2Loader.dll" "..\GM\WebView2.gml\extensions\WebView2\"
echo Copied build files