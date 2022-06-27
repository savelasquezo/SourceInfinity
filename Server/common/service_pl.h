//Find on end of file #endif and add before
/* OFFLINE SHOPS */
#define OFFLINE_SHOP // Offline shops system
#define GIFT_SYSTEM // gift system enable
#define SHOP_TIME_REFRESH 1*60 // time for cycle checking older shops
#define SHOP_MAX 2 //MAX SHOPS CREATED BY PC
#define SHOP_ADD_TIME 86400 // This will be add shops for +X days, don't touch
#define SHOP_BLOCK_GAME99 //Blocking create shops on channel 99
#define SHOP_DISTANCE // Show shops in pos distance like WoM2
#define SHOP_CURRENCY //Enable shop cash currency
#define SHOP_AUTO_CLOSE //Enable auto closing shop after sell last item
#define SHOP_LANG_PL
#define SHOP_GM_PRIVILEGES GM_IMPLEMENTOR //Minimum GM privileges to using Shop GM Panel

#define __SEND_TARGET_INFO__



#ifdef SHOP_LANG_PL
#define SHOP_SUCCESS_CLOSE "Sklep został zamknięty, pozostałe przedmioty oraz zarobione yang zostały dodane do podarków."
#define SHOP_MAX_ERR "Nie możesz stworzyć więcej sklepów"  
#define INVENTORY_FULL_ERROR "Nie masz miejsca w ekwipunku" 
#define UNKNOW_ERROR "Wystapił błąd" // 
#define SHOP_YANG_ADD "Yangi zostaną dodane do twojego ekwipunku"
#define SHOP_CASH_ADD "Smocze Monety zostały dodane do twojego konta"
#define SHOP_TIMEOUT "Proszę odczekać %ds zanim otworzysz kolejny sklep" 
#define GIFT_NOT_EXISTS "Przedmiot nie istnieje"
#define GIFT_ADD_SUCCESS "Przedmiot został dodany do twojego ekwipunku"
#define SHOP_REASON "Zamkniecie sklepu" // reason for gift gui
#define SHOP_ITEM_REASON "Usuniecie przedmiotu" // reason for gift gui
#define SHOP_NAME "Sklep #PLAYER_NAME# [#ID#]" ///Shops npc name
#define SHOP_PRIV_ERROR "Tworzenie sklepów dla twojej posady zostało zablokowane!"
#define SHOP_OX_ERROR "Nie możesz otworzyć sklepu tutaj." 
#define SHOP_MAP_MAX "Nie możesz stworzyć więcej sklepów na tej mapie."
#define SHOP_TEMP_OFF  "Tworzenie sklepów jest tymczasowo wyłączone"
#define SHOP_EDIT_SUCCESS "Przedmioty zostały pomyśnie zaktualizowane."
#endif