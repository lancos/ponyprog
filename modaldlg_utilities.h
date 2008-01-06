
extern int SetCommandObject(const int id, const int val, CommandObject* CmdList);
extern int SetCommandSensitive(const int id, const int val, CommandObject* CmdList);
extern int SetCommandLabel(const int id, char *str, CommandObject* CmdList);
extern int SetCommandHidden(const int id, const bool val, CommandObject* CmdList);
extern int SetCommandArrayHidden(const int id, const int n, const bool val, CommandObject* CmdList);
extern int SetCBelow(const int id, const ItemVal val, CommandObject* CmdList);

extern int GetCommandObject(const int id, CommandObject* CmdList);
