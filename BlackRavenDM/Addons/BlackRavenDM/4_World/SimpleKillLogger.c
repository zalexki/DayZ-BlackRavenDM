// Cant create/point to folder oO ?
static string DataFolderLocation = "$profile:Stats-";

class SimpleKillLogger
{
    ref map<string, string> data = new map<string, string>();

    void OnConnectHandler(string id, string name)
    {
        data = Load(id);
        data.Set("name", name);

        Save(data, id);
    }

    void DeathHandler(string id, vector pos)
    {
        data = Load(id);

        if (data.Contains("death")) {
            int death = data.Get("death").ToInt();
            death = death + 1;
            data.Set("death", death.ToString());
        } else {
            data.Set("death", "1");
        }

        data.Set("posDeath", pos[0].ToString() +","+pos[1].ToString()+","+pos[2].ToString());

        Save(data, id);
    }

    void KillHandler(string id, vector pos)
    {
        data = Load(id);

        if (data.Contains("kill")) {
            int kill = data.Get("kill").ToInt();
            kill = kill + 1;
            data.Set("kill", kill.ToString());
        } else {
            data.Set("kill", "1");
        }

        data.Set("posKill", pos[0].ToString() +","+pos[1].ToString()+","+pos[2].ToString());

        Save(data, id);
    }

    map<string, string> Load(string steamID)
    {
        ref map<string, string> data = new map<string, string>();
        string FileLocation = DataFolderLocation + steamID + ".json";

        if ( FileExist(FileLocation) ) {
            JsonFileLoader< map<string, string> >.JsonLoadFile(FileLocation, data);
        } else {
            JsonFileLoader< map<string, string> >.JsonSaveFile(FileLocation, data);
        }

        return data;
    }

    void Save(map<string, string> data, string steamID)
    {
        string FileLocation = DataFolderLocation + steamID + ".json";
        JsonFileLoader< map<string, string> >.JsonSaveFile(FileLocation, data);
    }
}
