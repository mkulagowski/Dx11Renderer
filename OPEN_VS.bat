@echo off
setlocal EnableExtensions EnableDelayedExpansion
for %%x in (devenv.exe) do if not [%%~$PATH:x]==[] (start "" /b "devenv.exe" .) else (start "" /b "E:\Apps\Microsoft Visual Studio\2017\Enterprise\Common7\IDE\devenv.exe" .)