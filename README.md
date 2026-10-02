# easyMenu

`easyMenu.inc` is a lightweight SA-MP include for creating singleplayer-style
menus with player textdraws. It provides a dialog-like callback API while
rendering the menu directly on the player's screen.

Menus support keyboard navigation, selection, cancellation, pagination, and
optional right-aligned information for each option.

## Requirements

- SA-MP server and `a_samp`
- A Pawn compiler compatible with player textdraws

Include `easyMenu` after `a_samp`:

```pawn
#include <a_samp>
#include <easyMenu>
```

## Features

| Feature                      | easyMenu |
|------------------------------|:--------:|
| Player textdraw interface    |   Yes    |
| Singleplayer-style layout    |   Yes    |
| Named menu callbacks         |   Yes    |
| Keyboard navigation          |   Yes    |
| Menu pagination              |   Yes    |
| Option information column    |   Yes    |
| Pause and resume             |   Yes    |
| Freeze the player while open |   Yes    |

## Creating a menu

Declare a menu callback with `Menu:` and open it with `Menu_Show`:

```pawn
Menu:WeaponMenu(playerid, response, row, option[])
{
    if (!response)
    {
        SendClientMessage(playerid, -1, "Weapon menu cancelled.");
        return 1;
    }

    new message[96];
    format(message, sizeof(message), "You selected row %d: %s", row, option);
    SendClientMessage(playerid, -1, message);
    return 1;
}

CMD:weapons(playerid, params[])
{
    Menu_Show(
        playerid,
        WeaponMenu,
        "Ammu-Nation",
        "Pistol\t$200\nDesert Eagle\t$1,200\nShotgun\t$900"
    );
    return 1;
}
```

The callback receives:

```pawn
Menu:MenuName(playerid, response, row, option[])
```

- `playerid` - The player who interacted with the menu.
- `response` - `1` when the player selects an option, or `0` when the menu is
  cancelled.
- `row` - The zero-based index of the selected option.
- `option[]` - The selected option's label.

## Option format

Separate options with `\n`. Use `\t` to add right-aligned information:

```pawn
Menu_Show(
    playerid,
    VehicleMenu,
    "Vehicles",
    "Infernus\t$100,000\nBanshee\t$85,000\nBullet\t$75,000"
);
```

The text before `\t` is passed to the callback as `option[]`. The text after
`\t` is displayed in the menu's information column.

## Controls

While a menu is open:

- `UP` and `DOWN` move between options.
- `SPRINT` confirms the selected option.
- `SECONDARY_ATTACK` cancels the menu.

The player is frozen while the menu is open. The include restores the player's
control state when the menu is closed and preserves the jetpack state.

## Menu functions

### `Menu_Show`

Opens a menu and resets the selection to the first option.

```pawn
Menu_Show(playerid, callback, title[], options[]);
```

The menu displays up to 10 rows at a time and automatically changes pages for
larger option lists.

### `EasyMenu_Open`

The underlying function used by `Menu_Show`:

```pawn
EasyMenu_Open(playerid, const function[], const title[], const options[]);
```

Use `Menu_Show` in normal code so the callback name is passed correctly.

### `EasyMenu_Close`

Closes the player's current menu and invokes its callback with `response` set
to `0`.

```pawn
EasyMenu_Close(playerid);
```

### `EasyMenu_IsOpen`

Returns `1` when a menu is open for the player, otherwise `0`.

```pawn
if (EasyMenu_IsOpen(playerid))
    EasyMenu_Close(playerid);
```

### `EasyMenu_Pause`

Pauses or resumes menu input. The menu remains visible while paused.

```pawn
EasyMenu_Pause(playerid);       // Pause
EasyMenu_Pause(playerid, false); // Resume
```

### `EasyMenu_IsPaused`

Returns `1` when the player's menu is paused, otherwise `0`.

```pawn
if (!EasyMenu_IsPaused(playerid))
    EasyMenu_Move(playerid, 1);
```

### `EasyMenu_Move`

Moves the selection by the specified number of rows. Positive values move
down; negative values move up.

```pawn
EasyMenu_Move(playerid, 1);
EasyMenu_Move(playerid, -1);
```

Selection wraps around when it reaches the first or last option.

## Optional callbacks

Define these callbacks only when they are needed:

```pawn
public EasyMenu_OnPlayerConnect(playerid)
{
    return 1;
}

public EasyMenu_OnPlayerDisconnect(playerid, reason)
{
    return 1;
}

public EasyMenu_OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    return 1;
}

public EasyMenu_OnPlayerUpdate(playerid)
{
    return 1;
}
```

The include also supports these optional input callbacks:

```pawn
public OnMenuRowChanged(playerid, row)
{
    return 1;
}

public OnPlayerKeyUpDown(playerid, updown)
{
    return 1;
}

public OnPlayerKeyLeftRight(playerid, leftright)
{
    return 1;
}
```

`OnMenuRowChanged` is called after the selected row changes. The key callbacks
are called when the corresponding key state changes, including when no menu is
open.

## Limits

- 10 visible rows per page.
- 64 options per menu.
- 36 characters for the title.
- 64 characters for each option label.
- 64 characters for each option information value.
- 32 characters for a callback name.

## Complete example

```pawn
#include <a_samp>
#include <easyMenu>

Menu:WeaponMenu(playerid, response, row, option[])
{
    if (!response)
    {
        SendClientMessage(playerid, -1, "Weapon shop closed.");
        return 1;
    }

    new message[96];
    format(message, sizeof(message), "You selected: %s.", option);
    SendClientMessage(playerid, -1, message);
    return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    if (!strcmp(cmdtext, "/weapons", true))
    {
        Menu_Show(
            playerid,
            WeaponMenu,
            "Ammu-Nation",
            "Pistol\t$200\nDesert Eagle\t$1,200\nShotgun\t$900\n"
            "Combat Shotgun\t$1,800\nMP5\t$2,500\nM4\t$4,500"
        );
        return 1;
    }
    return 0;
}
```

## Credits

Created for SA-MP gamemodes that need simple, reusable textdraw menus with a
singleplayer-inspired user interface.