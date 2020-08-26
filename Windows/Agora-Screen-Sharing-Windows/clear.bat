rmdir Debug /S /Q
rmdir Release /S /Q
rmdir ipch /S /Q
rmdir .vs /S /Q
pushd language

pushd English
rmdir Debug /S /Q
rmdir Release /S /Q
popd

pushd ChineseSimplified
rmdir Debug /S /Q
rmdir Release /S /Q
popd

popd

pushd OpenLive
rmdir Debug /S /Q
rmdir Release /S /Q
popd


pause