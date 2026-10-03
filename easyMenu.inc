/*
    easyMenu.inc - Textdraw menus with dialog-style callbacks.

    Usage:
        CMenu:Example(playerid, response, row, option[])
        {
            if (!response)
                return 1;

            printf("Selected row %d: %s", row, option);
            return 1;
        }

        Menu_Show(playerid, Example, "Example menu", "First option\nSecond option");
*/

#if defined _easyMenu_included
    #endinput
#endif
#define _easyMenu_included

#define EASY_MENU_VISIBLE_ROWS       (10)
#define EASY_MENU_MAX_OPTIONS        (64)
#define EASY_MENU_TITLE_LENGTH       (36)
#define EASY_MENU_OPTION_LENGTH      (64)
#define EASY_MENU_CALLBACK_LENGTH    (32)

#if !defined TEXT_DRAW_ALIGN_LEFT
    #define TEXT_DRAW_ALIGN_LEFT     (1)
    #define TEXT_DRAW_ALIGN_CENTER   (2)
    #define TEXT_DRAW_ALIGN_RIGHT    (3)
    #define TEXT_DRAW_FONT_0         (0)
    #define TEXT_DRAW_FONT_1         (1)
#endif

#if !defined PlayerTextDrawColour
    #define PlayerTextDrawColour     PlayerTextDrawColor
    #define PlayerTextDrawBackgroundColour PlayerTextDrawBackgroundColor
    #define PlayerTextDrawBoxColour   PlayerTextDrawBoxColor
#endif

#define CMenu:%0(%1) \
    forward menu_%0(%1); public menu_%0(%1)

#define Menu_Show(%0,%1, \
    EasyMenu_Open(%0, #%1,

static
    PlayerText:easyMenu_Box[MAX_PLAYERS],
    PlayerText:easyMenu_Title[MAX_PLAYERS],
    PlayerText:easyMenu_Items[MAX_PLAYERS][EASY_MENU_VISIBLE_ROWS],
    PlayerText:easyMenu_Info[MAX_PLAYERS][EASY_MENU_VISIBLE_ROWS],
    bool:easyMenu_Opened[MAX_PLAYERS],
    bool:easyMenu_Paused[MAX_PLAYERS],
    bool:easyMenu_DrawsCreated[MAX_PLAYERS],
    easyMenu_Selected[MAX_PLAYERS],
    easyMenu_Page[MAX_PLAYERS],
    easyMenu_Count[MAX_PLAYERS],
    easyMenu_LastUpDown[MAX_PLAYERS],
    easyMenu_LastLeftRight[MAX_PLAYERS],
    easyMenu_Tick[MAX_PLAYERS],
    easyMenu_TitleText[MAX_PLAYERS][EASY_MENU_TITLE_LENGTH],
    easyMenu_Callback[MAX_PLAYERS][EASY_MENU_CALLBACK_LENGTH],
    easyMenu_Options[MAX_PLAYERS][EASY_MENU_MAX_OPTIONS][EASY_MENU_OPTION_LENGTH],
    easyMenu_OptionInfo[MAX_PLAYERS][EASY_MENU_MAX_OPTIONS][EASY_MENU_OPTION_LENGTH],
    bool:easyMenu_UsingJetpack[MAX_PLAYERS]
;

stock EasyMenu_SetTextDefaults(playerid, PlayerText:text, Float:letter_x, Float:letter_y)
{
    PlayerTextDrawLetterSize(playerid, text, letter_x, letter_y);
    PlayerTextDrawAlignment(playerid, text, TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, text, -1);
    PlayerTextDrawSetShadow(playerid, text, 2);
    PlayerTextDrawSetOutline(playerid, text, 0);
    PlayerTextDrawBackgroundColour(playerid, text, 255);
    PlayerTextDrawFont(playerid, text, TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, text, true);
}

stock EasyMenu_CreateDraws(playerid)
{
    easyMenu_Box[playerid] = CreatePlayerTextDraw(playerid, 121.0, 140.0, "_");
    PlayerTextDrawLetterSize(playerid, easyMenu_Box[playerid], 0.300, 18.19);
    PlayerTextDrawTextSize(playerid, easyMenu_Box[playerid], 39.0, 183.0);
    PlayerTextDrawAlignment(playerid, easyMenu_Box[playerid], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, easyMenu_Box[playerid], -1);
    PlayerTextDrawUseBox(playerid, easyMenu_Box[playerid], true);
    PlayerTextDrawBoxColour(playerid, easyMenu_Box[playerid], 226);
    PlayerTextDrawSetShadow(playerid, easyMenu_Box[playerid], 1);
    PlayerTextDrawFont(playerid, easyMenu_Box[playerid], TEXT_DRAW_FONT_1);

    easyMenu_Title[playerid] = CreatePlayerTextDraw(playerid, 37.0, 125.0, "Menu");
    PlayerTextDrawLetterSize(playerid, easyMenu_Title[playerid], 0.759, 2.599);
    PlayerTextDrawColour(playerid, easyMenu_Title[playerid], -1);
    PlayerTextDrawSetShadow(playerid, easyMenu_Title[playerid], 1);
    PlayerTextDrawSetOutline(playerid, easyMenu_Title[playerid], 2);
    PlayerTextDrawBackgroundColour(playerid, easyMenu_Title[playerid], 255);
    PlayerTextDrawFont(playerid, easyMenu_Title[playerid], TEXT_DRAW_FONT_0);

    for (new i = 0; i < EASY_MENU_VISIBLE_ROWS; i++)
    {
        new Float:y = 155.0 + (i * 13.0);

        easyMenu_Items[playerid][i] = CreatePlayerTextDraw(playerid, 36.0, y, "_");
        EasyMenu_SetTextDefaults(playerid, easyMenu_Items[playerid][i], 0.409, 1.599);

        easyMenu_Info[playerid][i] = CreatePlayerTextDraw(playerid, 207.0, y, "_");
        PlayerTextDrawLetterSize(playerid, easyMenu_Info[playerid][i], 0.369, 1.599);
        PlayerTextDrawTextSize(playerid, easyMenu_Info[playerid][i], 12.0, 15.0);
        PlayerTextDrawAlignment(playerid, easyMenu_Info[playerid][i], TEXT_DRAW_ALIGN_RIGHT);
        PlayerTextDrawColour(playerid, easyMenu_Info[playerid][i], -1);
        PlayerTextDrawSetShadow(playerid, easyMenu_Info[playerid][i], 2);
        PlayerTextDrawBackgroundColour(playerid, easyMenu_Info[playerid][i], 255);
        PlayerTextDrawFont(playerid, easyMenu_Info[playerid][i], TEXT_DRAW_FONT_1);
        PlayerTextDrawSetProportional(playerid, easyMenu_Info[playerid][i], true);
    }
}

stock EasyMenu_ParseOptions(playerid, const options[])
{
    new length = strlen(options), start, count;

    while (start <= length && count < EASY_MENU_MAX_OPTIONS)
    {
        new end = start;
        while (end < length && options[end] != '\n')
            end++;

        new separator = start;
        while (separator < end && options[separator] != '\t')
            separator++;

        new optionLength = separator - start;
        if (optionLength >= EASY_MENU_OPTION_LENGTH)
            optionLength = EASY_MENU_OPTION_LENGTH - 1;
        for (new i = 0; i < optionLength; i++)
            easyMenu_Options[playerid][count][i] = options[start + i];
        easyMenu_Options[playerid][count][optionLength] = EOS;

        new infoLength;
        if (separator < end)
        {
            infoLength = end - separator - 1;
            if (infoLength >= EASY_MENU_OPTION_LENGTH)
                infoLength = EASY_MENU_OPTION_LENGTH - 1;
            for (new i = 0; i < infoLength; i++)
                easyMenu_OptionInfo[playerid][count][i] = options[separator + 1 + i];
        }
        easyMenu_OptionInfo[playerid][count][infoLength] = EOS;

        if (optionLength > 0 && easyMenu_Options[playerid][count][optionLength - 1] == '\r')
            easyMenu_Options[playerid][count][optionLength - 1] = EOS;
        if (infoLength > 0 && easyMenu_OptionInfo[playerid][count][infoLength - 1] == '\r')
            easyMenu_OptionInfo[playerid][count][infoLength - 1] = EOS;

        count++;
        start = end + 1;
    }

    easyMenu_Count[playerid] = count;
    return count;
}

stock EasyMenu_Render(playerid)
{
    new start = easyMenu_Page[playerid] * EASY_MENU_VISIBLE_ROWS;
    new visible = easyMenu_Count[playerid] - start;
    if (visible > EASY_MENU_VISIBLE_ROWS)
        visible = EASY_MENU_VISIBLE_ROWS;

    PlayerTextDrawLetterSize(playerid, easyMenu_Box[playerid], 0.300, 3.19 + (visible * 1.5));
    PlayerTextDrawSetString(playerid, easyMenu_Title[playerid], easyMenu_TitleText[playerid]);
    PlayerTextDrawShow(playerid, easyMenu_Title[playerid]);
    PlayerTextDrawShow(playerid, easyMenu_Box[playerid]);

    for (new i = 0; i < EASY_MENU_VISIBLE_ROWS; i++)
    {
        if (i < visible)
        {
            new option = start + i;
            PlayerTextDrawSetString(playerid, easyMenu_Items[playerid][i], easyMenu_Options[playerid][option]);
            PlayerTextDrawSetString(playerid, easyMenu_Info[playerid][i], easyMenu_OptionInfo[playerid][option]);
            PlayerTextDrawColour(playerid, easyMenu_Items[playerid][i], option == easyMenu_Selected[playerid] ? -1867921409 : 1028282623);
            PlayerTextDrawColour(playerid, easyMenu_Info[playerid][i], option == easyMenu_Selected[playerid] ? -1867921409 : 1028282623);
            PlayerTextDrawHide(playerid, easyMenu_Items[playerid][i]);
            PlayerTextDrawShow(playerid, easyMenu_Items[playerid][i]);
            if (easyMenu_OptionInfo[playerid][option][0])
            {
                PlayerTextDrawHide(playerid, easyMenu_Info[playerid][i]);
                PlayerTextDrawShow(playerid, easyMenu_Info[playerid][i]);
            }
            else
                PlayerTextDrawHide(playerid, easyMenu_Info[playerid][i]);
        }
        else
        {
            PlayerTextDrawSetString(playerid, easyMenu_Items[playerid][i], "_");
            PlayerTextDrawSetString(playerid, easyMenu_Info[playerid][i], "_");
            PlayerTextDrawHide(playerid, easyMenu_Items[playerid][i]);
            PlayerTextDrawHide(playerid, easyMenu_Info[playerid][i]);
        }
    }
}

stock EasyMenu_HideDraws(playerid)
{
    PlayerTextDrawHide(playerid, easyMenu_Title[playerid]);
    PlayerTextDrawHide(playerid, easyMenu_Box[playerid]);
    for (new i = 0; i < EASY_MENU_VISIBLE_ROWS; i++)
    {
        PlayerTextDrawSetString(playerid, easyMenu_Items[playerid][i], "_");
        PlayerTextDrawSetString(playerid, easyMenu_Info[playerid][i], "_");
        PlayerTextDrawHide(playerid, easyMenu_Items[playerid][i]);
        PlayerTextDrawHide(playerid, easyMenu_Info[playerid][i]);
    }
}

stock EasyMenu_Dispatch(playerid, response)
{
    if (!easyMenu_Opened[playerid])
        return 0;

    new callback[EASY_MENU_CALLBACK_LENGTH], option[EASY_MENU_OPTION_LENGTH];
    new row = easyMenu_Selected[playerid];
    format(callback, sizeof(callback), "menu_%s", easyMenu_Callback[playerid]);
    if (row >= 0 && row < easyMenu_Count[playerid])
        format(option, sizeof(option), easyMenu_Options[playerid][row]);

    EasyMenu_HideDraws(playerid);
    TogglePlayerControllable(playerid, true);
    
    if (easyMenu_UsingJetpack[playerid])
        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USEJETPACK);

    easyMenu_Opened[playerid] = false;
    easyMenu_UsingJetpack[playerid] = false;
    easyMenu_Callback[playerid][0] = EOS;

    if(response)
        PlayerPlaySound(playerid, 1054, 0.0, 0.0, 0.0);
    else
        PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);

    if (funcidx(callback) != -1)
        CallLocalFunction(callback, "ddds", playerid, response, row, option);
    return 1;
}

stock EasyMenu_Open(playerid, const function[], const title[], const options[])
{
    if (!strlen(options))
        return 0;
    if (!easyMenu_DrawsCreated[playerid])
    {
        EasyMenu_CreateDraws(playerid);
        easyMenu_DrawsCreated[playerid] = true;
    }
    if (easyMenu_Opened[playerid])
        EasyMenu_Dispatch(playerid, 0);

    format(easyMenu_Callback[playerid], EASY_MENU_CALLBACK_LENGTH, function);
    format(easyMenu_TitleText[playerid], EASY_MENU_TITLE_LENGTH, title);
    EasyMenu_ParseOptions(playerid, options);
    if (!easyMenu_Count[playerid])
        return 0;

    easyMenu_Selected[playerid] = 0;
    easyMenu_Page[playerid] = 0;
    easyMenu_Opened[playerid] = true;
    easyMenu_Paused[playerid] = false;
    easyMenu_UsingJetpack[playerid] = GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_USEJETPACK;

    easyMenu_Tick[playerid] = GetTickCount();

    TogglePlayerControllable(playerid, false);
    EasyMenu_Render(playerid);
    return 1;
}

stock EasyMenu_Close(playerid)
{
    return EasyMenu_Dispatch(playerid, 0);
}

stock EasyMenu_IsOpen(playerid)
{
    return easyMenu_Opened[playerid];
}

stock EasyMenu_Pause(playerid, bool:paused = true)
{
    if (!easyMenu_Opened[playerid])
        return 0;

    easyMenu_Paused[playerid] = paused;
    return 1;
}

stock EasyMenu_IsPaused(playerid)
{
    return easyMenu_Paused[playerid];
}

stock EasyMenu_Move(playerid, direction)
{
    if (!easyMenu_Opened[playerid] || easyMenu_Paused[playerid])
        return 0;

    new next = easyMenu_Selected[playerid] + direction;
    if (next < 0)
        next = easyMenu_Count[playerid] - 1;
    else if (next >= easyMenu_Count[playerid])
        next = 0;

    if(next != easyMenu_Selected[playerid])
        PlayerPlaySound(playerid, 1083, 0.0, 0.0, 0.0);
    else
        PlayerPlaySound(playerid, 1085, 0.0, 0.0, 0.0);

    easyMenu_Selected[playerid] = next;
    easyMenu_Page[playerid] = next / EASY_MENU_VISIBLE_ROWS;


    EasyMenu_Render(playerid);

    if (funcidx("OnMenuRowChanged") != -1)
        CallLocalFunction("OnMenuRowChanged", "dd", playerid, next);
    return 1;
}

stock Menu_SetRow(playerid, row)
{
    if (!easyMenu_Opened[playerid] || easyMenu_Paused[playerid])
        return 0;

    if (row < 0 || row >= easyMenu_Count[playerid])
        return 0;

    easyMenu_Selected[playerid] = row;
    easyMenu_Page[playerid] = row / EASY_MENU_VISIBLE_ROWS;

    EasyMenu_Render(playerid);

    if (funcidx("OnMenuRowChanged") != -1)
        CallLocalFunction("OnMenuRowChanged", "dd", playerid, row);
    return 1;
}

public OnPlayerConnect(playerid)
{
    EasyMenu_CreateDraws(playerid);
    easyMenu_Opened[playerid] = false;
    easyMenu_Paused[playerid] = false;
    easyMenu_DrawsCreated[playerid] = true;
    easyMenu_LastUpDown[playerid] = 0;
    easyMenu_LastLeftRight[playerid] = 0;
    if (funcidx("EasyMenu_OnPlayerConnect") != -1)
        return CallLocalFunction("EasyMenu_OnPlayerConnect", "i", playerid);
    return 1;
}

#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect EasyMenu_OnPlayerConnect
#if defined EasyMenu_OnPlayerConnect
    forward EasyMenu_OnPlayerConnect(playerid);
#endif

public OnPlayerDisconnect(playerid, reason)
{
    #pragma unused reason
    if (easyMenu_Opened[playerid])
        EasyMenu_Dispatch(playerid, 0);

    PlayerTextDrawDestroy(playerid, easyMenu_Box[playerid]);
    PlayerTextDrawDestroy(playerid, easyMenu_Title[playerid]);
    for (new i = 0; i < EASY_MENU_VISIBLE_ROWS; i++)
    {
        PlayerTextDrawDestroy(playerid, easyMenu_Items[playerid][i]);
        PlayerTextDrawDestroy(playerid, easyMenu_Info[playerid][i]);
    }
    easyMenu_DrawsCreated[playerid] = false;
    easyMenu_Paused[playerid] = false;
    #if defined EasyMenu_OnPlayerDisconnect
        return EasyMenu_OnPlayerDisconnect(playerid, reason);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect EasyMenu_OnPlayerDisconnect
#if defined EasyMenu_OnPlayerDisconnect
    forward EasyMenu_OnPlayerDisconnect(playerid, reason);
#endif

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if (easyMenu_Opened[playerid] && !easyMenu_Paused[playerid] && GetTickCount() - easyMenu_Tick[playerid] > 400)
    {
        if ((newkeys & KEY_SPRINT) && !(oldkeys & KEY_SPRINT))
            EasyMenu_Dispatch(playerid, 1);
        else if ((newkeys & KEY_SECONDARY_ATTACK) && !(oldkeys & KEY_SECONDARY_ATTACK))
            EasyMenu_Dispatch(playerid, 0);
    }

    #if defined EasyMenu_OnPlayerKeyStateChange
        return EasyMenu_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerKeyStateChange
    #undef OnPlayerKeyStateChange
#else
    #define _ALS_OnPlayerKeyStateChange
#endif

#define OnPlayerKeyStateChange EasyMenu_OnPlayerKeyStateChange

#if defined EasyMenu_OnPlayerKeyStateChange
    forward EasyMenu_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
#endif


public OnPlayerUpdate(playerid)
{
    new keys, updown, leftright;
    GetPlayerKeys(playerid, keys, updown, leftright);

    if (updown != easyMenu_LastUpDown[playerid] && GetTickCount() - easyMenu_Tick[playerid] > 400)
    {
        easyMenu_LastUpDown[playerid] = updown;

        if (easyMenu_Opened[playerid] && !easyMenu_Paused[playerid])
        {
            if (updown == KEY_UP)
                EasyMenu_Move(playerid, -1);
            else if (updown == KEY_DOWN)
                EasyMenu_Move(playerid, 1);
        }
    }

    #if defined EasyMenu_OnPlayerUpdate
        return EasyMenu_OnPlayerUpdate(playerid);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerUpdate
    #undef OnPlayerUpdate
#else
    #define _ALS_OnPlayerUpdate
#endif
#define OnPlayerUpdate EasyMenu_OnPlayerUpdate
#if defined EasyMenu_OnPlayerUpdate
    forward EasyMenu_OnPlayerUpdate(playerid);
#endif