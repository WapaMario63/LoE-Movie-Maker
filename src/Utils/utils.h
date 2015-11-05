#ifndef UTILS_H
#define UTILS_H

void saveResourceToDataFolder(QString resRelPath);
QByteArray removeHTTPHeader(QByteArray data,QString header);
char convertChar (char c, bool direction = true);
float timestampNow();

//#if defined BABSCON15
class Utilities
{
public:
    enum Utils
    {
        UtilMessageBox = 0x55,
        UtilSceneAreas = 80,
        UtilRoomBounds = 70
    };
    enum MessageBoxButtons
    {
        MsgBoxBtn_Invalid = 0,
        MsgBoxBtn_OkayCancel = 2,
        MsgBoxBtn_YesCancel = 3,
        MsgBoxBtn_YesNo = 1,
        MsgBoxBtn_YesNoCancel = 4
    };
    enum MessageBoxResult
    {
        MsgBoxRes_Cancel = 2,
        MsgBoxRes_Invalid = 0,
        MsgBoxRes_No = 1,
        MsgBoxRes_Ok = 3,
        MsgBoxRes_Yes = 4
    };
};
//#endif
#endif // UTILS_H
