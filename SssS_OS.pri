## split up VERSION to then join again without periods
SssS_APP_VERSION_DOTTED = "$$VERSION"
SssS_APP_VERSION_SPLIT = $$split(VERSION, ".")
SssS_APP_VERSION = $$join(SssS_APP_VERSION_SPLIT, "_")

## determine if we are building for Raspberry Pi
#message($$QMAKESPEC)
SssSisRaspberryPiCheck = $$find(QMAKESPEC, linux-rasp-pi)
!isEmpty(SssSisRaspberryPiCheck) {
#message("RasPi")
	CONFIG *= SssS_RasPi
	DEFINES *= SssS_RasPi
	SssS_RasPiV = 0
	## determine which version
	SssSisRaspberryPiCheck3 = $$find(QMAKESPEC, linux-rasp-pi3)
	SssSisRaspberryPiCheck2 = $$find(QMAKESPEC, linux-rasp-pi2)
	!isEmpty(SssSisRaspberryPiCheck3) {
		CONFIG *= SssS_RasPiV3
		DEFINES *= SssS_RasPiV3
		SssS_RasPiV = 3
	} else:!isEmpty(SssSisRaspberryPiCheck2) {
		CONFIG *= SssS_RasPiV2
		DEFINES *= SssS_RasPiV2
		SssS_RasPiV = 2
	} else {
		CONFIG *= SssS_RasPiV1
		DEFINES *= SssS_RasPiV1
		SssS_RasPiV = 1
	}
	DEFINES *= SssS_RasPiV
} ## if got RasPi

SssS_TARGET_BITS = "64"

## linux
unix:!mac:!android {
#message("Linux")
	DEFINES *= SssS_OS_Linux
	SssS_TARGET_PLATFORM = "L"
}

## RasPi linux
unix:!mac:!android:CONFIG(SssS_RasPi) {
	DEFINES *= SssS_OS_RasPi
	SssS_TARGET_BITS = "32"
	SssS_TARGET_PLATFORM = R$${SssS_RasPiV}
}

## android
android {
#message("Android")
	DEFINES *= SssS_OS_Android
	SssS_TARGET_PLATFORM = "A"
	SssS_TARGET_BITS = "32"
}

## OSX & iOS
mac {
	DEFINES *= SssS_OS_Mac
	macx {
	message("OSX")
		DEFINES *= SssS_OS_OSX
		SssS_TARGET_PLATFORM = "X"
	} else {
	message("iOS")
		DEFINES *= SssS_OS_iOS
		SssS_TARGET_PLATFORM = "i"
		SssS_TARGET_BITS = "32"
}	}

## Windows
win {
message("Windows")
	DEFINES *= SssS_OS_WIN
	SssS_TARGET_PLATFORM = "W"
}

contains(DEFINES, SssS_USE_GUI) {
	SssS_TARGET_GUI = GUI
} else {
	SssS_TARGET_GUI = CLI
}

## assemble the executable name
SssS_APP_NAME_FULL = $${SssS_APP_NAME}_$${SssS_TARGET_BITS}$${SssS_TARGET_PLATFORM}_$${SssS_TARGET_GUI}_V$${SssS_APP_VERSION}

## add some values to DEFINES
DEFINES *= SssS_APP_NAME=\\\"$$SssS_APP_NAME\\\"
DEFINES *= SssS_APP_NAME_FULL=\\\"$$SssS_APP_NAME_FULL\\\"
DEFINES *= SssS_APP_VERSION=\\\"$$SssS_APP_VERSION\\\"
DEFINES *= SssS_APP_VERSION_DOTTED=\\\"$$SssS_APP_VERSION_DOTTED\\\"
