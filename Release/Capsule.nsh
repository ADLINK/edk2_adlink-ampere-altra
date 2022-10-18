@echo -off
set pkgver %VER%.%BUILD% # will be updated

BoardVersion.efi
if %lasterror% == 0xA1 then
   echo "BoardRevision: A1"
   set capver ".A1."
else   
   echo "BoardRevision: A2"
   set capver "."
endif 

echo "Update SCP"
CapsuleApp.efi ComHpcAlt_scp_%pkgver%%capver%cap
echo "Update EDK2"
CapsuleApp.efi ComHpcAlt_tianocore_atf_%pkgver%%capver%cap
echo "Reboot to firmware UI"
FwUi.efi
