rm -f *.ipa
rm -rf *.app
rm -f *.zip
rm -rf dSYMs
rm -rf *.dSYM
rm -f *dSYMs.zip
rm -rf *.xcarchive

BUILD_DATE=`date +%Y-%m-%d-%H.%M.%S`
APP_TARGET=Agora-Video-With-ARKit
ArchivePath=${APP_TARGET}-${BUILD_DATE}.xcarchive

xcodebuild clean -project "${APP_TARGET}.xcodeproj" -scheme "${APP_TARGET}" -configuration Release
xcodebuild -project "${APP_TARGET}.xcodeproj" -scheme "${APP_TARGET}" -archivePath ${ArchivePath} archive
xcodebuild -exportArchive -exportOptionsPlist exportPlist.plist -archivePath ${ArchivePath} -exportPath .