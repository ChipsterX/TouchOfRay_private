#pragma once

class CUIOptionsManagerScript
{
public:
    void SaveBackupValues(pcstr group);
    void SetCurrentValues(pcstr group);
    void SaveValues(pcstr group);
    void UndoGroup(pcstr group);
    void OptionsPostAccept();
    void SendMessage2Group(pcstr group, pcstr message);
	void DoLevelRestart();
    bool NeedSystemRestart();
    bool NeedVidRestart();
	bool NeedLevelRestart();
};
