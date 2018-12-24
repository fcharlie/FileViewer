
// Generate code by cmake, don't modify
#ifndef BASE_VERSION_H
#define BASE_VERSION_H

#define FILEVIEW_VERSION L"${FILEVIEW_VERSION_MAJOR}.${FILEVIEW_VERSION_MINOR}.${FILEVIEW_VERSION_PATCH}"
#define FILEVIEW_HASH L"${GIT_SHA1}"
#define FILEVIEW_REFS L"${GIT_REFSPEC}"

#define FILEVIEW_APPNAME L"FILEVIEW-${FILEVIEW_VERSION_MAJOR}.${FILEVIEW_VERSION_MINOR}"

#endif
