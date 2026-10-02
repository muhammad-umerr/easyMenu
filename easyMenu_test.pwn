#include <a_samp>
#include <easyMenu>

forward EasyMenu_OnPlayerConnect(playerid);
forward OnMenuRowChanged(playerid, row);

static const WeaponNames[][] =
{
    "Pistol",
    "Desert Eagle",
    "Shotgun",
    "Combat Shotgun",
    "Micro SMG",
    "MP5",
    "AK-47",
    "M4",
    "Country Rifle",
    "Sniper Rifle",
    "RPG",
    "Knife"
};

static const WeaponPrices[] =
{
    200,
    1200,
    900,
    1800,
    1000,
    2500,
    3500,
    4500,
    2000,
    7500,
    15000,
    350
};

static SelectedWeapon[MAX_PLAYERS] = {-1, ...};

public OnFilterScriptInit()
{
    print("easyMenu test filterscript loaded.");
    return 1;
}

public OnPlayerConnect(playerid)
{
    SelectedWeapon[playerid] = -1;
    SendClientMessage(playerid, -1, "Type /easymenu to open the Ammu-Nation test menu.");
    return 1;
}

public OnMenuRowChanged(playerid, row)
{
    new message[64];
    format(message, sizeof(message), "Active weapon row: %d", row);
    SendClientMessage(playerid, 0xBFC0C2FF, message);
    return 1;
}

Menu:generalHandler(playerid, response, row, option[])
{
    new menu[192], message[128];

    if (!response)
    {
        if (SelectedWeapon[playerid] != -1)
        {
            SelectedWeapon[playerid] = -1;
            Menu_Show(playerid, generalHandler, "Ammu-Nation", "Pistol\t$200\nDesert Eagle\t$1,200\nShotgun\t$900\nCombat Shotgun\t$1,800\nMicro SMG\t$1,000\nMP5\t$2,500\nAK-47\t$3,500\nM4\t$4,500\nCountry Rifle\t$2,000\nSniper Rifle\t$7,500\nRPG\t$15,000\nKnife\t$350");
        }
        else
            SendClientMessage(playerid, -1, "Weapon shop closed.");
        return 1;
    }

    if (SelectedWeapon[playerid] == -1)
    {
        if (row < 0 || row >= sizeof(WeaponNames))
            return 1;

        SelectedWeapon[playerid] = row;
        format(menu, sizeof(menu), "Buy %s\t$%d\nBuy Ammo (x30)\t$%d", WeaponNames[row], WeaponPrices[row], WeaponPrices[row] / 10);
        Menu_Show(playerid, generalHandler, WeaponNames[row], menu);
        return 1;
    }

    if (row == 0)
        format(message, sizeof(message), "You selected: buy %s.", WeaponNames[SelectedWeapon[playerid]]);
    else
        format(message, sizeof(message), "You selected: buy ammo for %s.", WeaponNames[SelectedWeapon[playerid]]);

    SendClientMessage(playerid, -1, message);
    SelectedWeapon[playerid] = -1;
    return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    if (!strcmp(cmdtext, "/easymenu", true))
    {
        SelectedWeapon[playerid] = -1;
        Menu_Show(playerid, generalHandler, "Ammu-Nation", "Pistol\t$200\nDesert Eagle\t$1,200\nShotgun\t$900\nCombat Shotgun\t$1,800\nMicro SMG\t$1,000\nMP5\t$2,500\nAK-47\t$3,500\nM4\t$4,500\nCountry Rifle\t$2,000\nSniper Rifle\t$7,500\nRPG\t$15,000\nKnife\t$350");
        return 1;
    }
    return 0;
}
