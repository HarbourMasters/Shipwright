#include "OTRGlobals.h"
#include <libultraship/libultraship.h>
#include "soh/resource/type/Scene.h"
#include <Utils/StringHelper.h>
#include "global.h"
#include "vt.h"
#include "soh/resource/type/Text.h"
#include <message_data_static.h>
#include "Enhancements/custom-message/CustomMessageManager.h"
#include "Enhancements/custom-message/CustomMessageTypes.h"

extern "C" MessageTableEntry* sNesMessageEntryTablePtr;
extern "C" MessageTableEntry* sGerMessageEntryTablePtr;
extern "C" MessageTableEntry* sFraMessageEntryTablePtr;
extern "C" MessageTableEntry* sStaffMessageEntryTablePtr;
//extern "C" MessageTableEntry* _message_0xFFFC_nes;  

MessageTableEntry* OTRMessage_LoadTable(const char* filePath, bool isNES) {
    auto file = std::static_pointer_cast<LUS::Text>(LUS::Context::GetInstance()->GetResourceManager()->LoadResource(filePath));

    if (file == nullptr)
        return nullptr;
    
    // Allocate room for an additional message
    // OTRTODO: Should not be malloc'ing here. It's fine for now since we check elsewhere that the message table is
    // already null.
    MessageTableEntry* table = (MessageTableEntry*)malloc(sizeof(MessageTableEntry) * (file->messages.size() + 1));

    for (size_t i = 0; i < file->messages.size(); i++) {
        // Look for Owl Text
        if (file->messages[i].id == 0x2066) {
            // Create a new message based on the Owl Text
            uint32_t kaeporaMsgSize = file->messages[i].msg.size();
            // OTRTODO: Should not be malloc'ing here. It's fine for now since we check elsewhere that the message table
            // is already null.
            char* kaeporaOg = (char*)malloc(sizeof(char) * kaeporaMsgSize);
            char* kaeporaPatch = (char*)malloc(sizeof(char) * kaeporaMsgSize);
            file->messages[i].msg.copy(kaeporaOg, kaeporaMsgSize, 0);
            file->messages[i].msg.copy(kaeporaPatch, kaeporaMsgSize, 0);

            size_t colorPos = file->messages[i].msg.find(QM_GREEN);
            size_t newLinePos = colorPos + file->messages[i].msg.substr(colorPos + 1).find(CTRL_NEWLINE) + 1;
            size_t endColorPos = newLinePos + file->messages[i].msg.substr(newLinePos).find(CTRL_COLOR);
            size_t NoLength = newLinePos - (colorPos + 1);
            size_t YesLength = endColorPos - (newLinePos + 1);
            // Swap the order of yes and no in this new message
            size_t yes = 0;
            while (yes < YesLength) {
                kaeporaPatch[colorPos + yes + 1] = kaeporaOg[newLinePos + yes + 1];
                yes++;
            }
            kaeporaPatch[colorPos + yes + 1] = CTRL_NEWLINE;
            size_t no = 0;
            while (no < NoLength) {
                kaeporaPatch[colorPos + yes + 2 + no] = kaeporaOg[colorPos + 1 + no];
                no++;
            }

            // load data into message
            table[file->messages.size()].textId = 0x71B3;
            table[file->messages.size()].typePos = (file->messages[i].textboxType << 4) | file->messages[i].textboxYPos;
            table[file->messages.size()].segment = kaeporaPatch;
            table[file->messages.size()].msgSize = kaeporaMsgSize;
        }

        table[i].textId = file->messages[i].id;
        table[i].typePos = (file->messages[i].textboxType << 4) | file->messages[i].textboxYPos;
        table[i].segment = file->messages[i].msg.c_str();
        table[i].msgSize = file->messages[i].msg.size();

        if (isNES && file->messages[i].id == 0xFFFC)
            _message_0xFFFC_nes = (char*)file->messages[i].msg.c_str();
    }

    // Assert that the first message starts at the first text ID
    assert(table[0].textId == 0x0001);

    return table;
}

extern "C" void OTRMessage_Init()
{
    // OTRTODO: Added a lot of null checks here so that we don't malloc the table multiple times causing a memory leak.
    // We really ought to fix the implementation such that we aren't malloc'ing new tables.
    // Once we fix the implementation, remove these NULL checks.
    if (sNesMessageEntryTablePtr == NULL) {
        sNesMessageEntryTablePtr = OTRMessage_LoadTable("text/nes_message_data_static/nes_message_data_static", true);
    }
    if (sGerMessageEntryTablePtr == NULL) {
        sGerMessageEntryTablePtr = OTRMessage_LoadTable("text/ger_message_data_static/ger_message_data_static", false);
    }
    if (sFraMessageEntryTablePtr == NULL) {
        sFraMessageEntryTablePtr = OTRMessage_LoadTable("text/fra_message_data_static/fra_message_data_static", false);
    }

    if (sStaffMessageEntryTablePtr == NULL) {
        auto file2 =
            std::static_pointer_cast<LUS::Text>(LUS::Context::GetInstance()->GetResourceManager()->LoadResource(
                "text/staff_message_data_static/staff_message_data_static"));
        // OTRTODO: Should not be malloc'ing here. It's fine for now since we check that the message table is already null.
        sStaffMessageEntryTablePtr = (MessageTableEntry*)malloc(sizeof(MessageTableEntry) * file2->messages.size());

        for (size_t i = 0; i < file2->messages.size(); i++) {
            sStaffMessageEntryTablePtr[i].textId = file2->messages[i].id;
            sStaffMessageEntryTablePtr[i].typePos =
                (file2->messages[i].textboxType << 4) | file2->messages[i].textboxYPos;
            sStaffMessageEntryTablePtr[i].segment = file2->messages[i].msg.c_str();
            sStaffMessageEntryTablePtr[i].msgSize = file2->messages[i].msg.size();
        }

        // Assert staff credits start at the first credits ID
        assert(sStaffMessageEntryTablePtr[0].textId == 0x0500);
    }

    CustomMessageManager::Instance->AddCustomMessageTable(customMessageTableID);
    CustomMessageManager::Instance->CreateGetItemMessage(
        customMessageTableID, (GetItemID)TEXT_GS_NO_FREEZE, ITEM_SKULL_TOKEN,
        CustomMessage("You got a %rGold Skulltula Token%w!&You've collected %r{{gsCount}}%w tokens&in total!\x0E\x3C",
                      "Ein %rGoldenes Skulltula-Symbol%w!&Du hast nun insgesamt %r{{gsCount}}&%wGoldene "
                      "Skulltula-Symbole&gesammelt!\x0E\x3C",
                      "Vous obtenez un %rSymbole de&Skulltula d'or%w! Vous avez&collecté %r{{gsCount}}%w symboles en "
                      "tout!\x0E\x3C",
                      TEXTBOX_TYPE_BLUE));
    CustomMessageManager::Instance->CreateGetItemMessage(
        customMessageTableID, (GetItemID)TEXT_GS_FREEZE, ITEM_SKULL_TOKEN,
        CustomMessage(
            "You got a %rGold Skulltula Token%w!&You've collected %r{{gsCount}}%w tokens&in total!",
            "Ein %rGoldenes Skulltula-Symbol%w!&Du hast nun insgesamt %r{{gsCount}}&%wGoldene "
            "Skulltula-Symbole&gesammelt!",
            "Vous obtenez un %rSymbole de&Skulltula d'or%w! Vous avez&collecté %r{{gsCount}}%w symboles en tout!",
            TEXTBOX_TYPE_BLUE));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BUY_BOMBCHU_10_DESC,
        CustomMessage("\x08%rBombchu  (10 pieces)  99 Rupees&%wThis looks like a toy mouse, but&it's actually a "
                      "self-propelled time&bomb!\x09\x0A",
                      "\x08%rKrabbelmine  10 Stück  99 Rubine&%wDas ist eine praktische Zeitbombe,&die Du als "
                      "Distanzwaffe&einsetzen kannst!\x09\x0A",
                      "\x08%rMissile  10 unités  99 Rubis&%wProfilée comme une souris&mécanique, cette arme est "
                      "&destructrice!!!\x09\x0A"));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BUY_BOMBCHU_10_PROMPT,
        CustomMessage("\x08"
                      "Bombchu  10 pieces   99 Rupees\x09&&\x1B%gBuy&Don't buy%w",
                      "\x08Krabbelmine  10 Stück  99 Rubine\x09&&\x1B%gKaufen!&Nicht kaufen!%w",
                      "\x08Missiles  10 unités   99 Rubis\x09&&\x1B%gAcheter&Ne pas acheter%w"));
    CustomMessageManager::Instance->CreateGetItemMessage(
        customMessageTableID, (GetItemID)TEXT_HEART_CONTAINER, ITEM_HEART_CONTAINER,
        CustomMessage(
            "You got a %rHeart Container%w!&You've collected %r{{heartContainerCount}}%w containers&in total!",
            "Ein %rHerzcontainer%w!&Du hast nun insgesamt %r{{heartContainerCount}}%w&Herzcontainer gesammelt!",
            "Vous obtenez un %rCoeur&d'Energie%w! Vous en avez&collecté %r{{heartContainerCount}}%w en tout!"));
    CustomMessageManager::Instance->CreateGetItemMessage(
        customMessageTableID, (GetItemID)TEXT_HEART_PIECE, ITEM_HEART_PIECE,
        CustomMessage("You got a %rHeart Piece%w!&You've collected %r{{heartPieceCount}}%w pieces&in total!",
                      "Ein %rHerzteil%w!&Du hast nun insgesamt %r{{heartPieceCount}}%w&Herteile gesammelt!",
                      "Vous obtenez un %rQuart de&Coeur%w! Vous en avez collecté&%r{{heartPieceCount}}%w en tout!",
                      TEXTBOX_TYPE_BLUE));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_MARKET_GUARD_NIGHT,
        CustomMessage("You look bored. Wanna go out for a&walk?\x1B&%gYes&No%w",
                      "Du siehst gelangweilt aus.&Willst du einen Spaziergang machen?\x1B&%gJa&Nein%w",
                      "Tu as l'air de t'ennuyer. Tu veux&aller faire un tour?\x1B&%gOui&Non%w"));

    const char* THREE_WAY_CHOICE = "\x1C";
    
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BANKER_GREETING,
        CustomMessage("Hey there, @!&You making a deposit?&Or are you here to make a&withdrawal?",
                    "Hallo, @!&Machst du eine Einzahlung?&Oder möchtest du etwas&abheben?",
                    "Salut, @!&Tu fais un dépôt ?&Ou es-tu là pour faire un&retrait ?"));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BANKER_OPTIONS,
        CustomMessage(std::string("What would you like to do?") + THREE_WAY_CHOICE + "&%gDeposit Rupees&%gWithdrawal Rupees&%gNothing%w",
                    std::string("Was möchtest du tun?") + THREE_WAY_CHOICE + "&%gRubine einzahlen&%gRubine abheben&%gNichts%w",
                    std::string("Que souhaitez-vous faire ?") + THREE_WAY_CHOICE + "&%gDéposer des rubis&%gRetirer des rubis&%gRien%w"));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BANKER_BALANCE,
        CustomMessage("If I remember, as of your&last warp transfer your&balance is %y{{playerBalance}}%w rupees.",
                      "Wenn ich mich recht erinnere, beträgt&dein Guthaben seit deiner&letzten Warpenübertragung %y{{playerBalance}}%w Rubine.",
                      "Si je me souviens bien, depuis votre&dernier transfert par warp, votre&solde est de %y{{playerBalance}}%w rubis."));
    CustomMessageManager::Instance->CreateMessage(
            customMessageTableID, TEXT_BANKER_WITHDRAWAL_AMOUNT,
            CustomMessage("\x08Select Withdrawal Amount:&     rupees.&Set the amount with \xAA and&press \x9F to decide.\x0A",
                        "\x08Wähle den Abhebungsbetrag:&     Rubine.&Stelle den Betrag mit \xAA ein und&drücke \x9F, um zu entscheiden.\x0A",
                        "\x08Sélectionnez le montant du retrait :&     rubis.&Réglez le montant avec \xAA et&appuyez sur \x9F pour décider.\x0A"));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BANKER_WITHDRAWAL_CONFIRM,
        CustomMessage("Here are your rupees.&Your new balance is:&%y{{playerBalance}}%w rupees.&See you later!",
                      "Hier sind deine Rubine.&Dein neues Guthaben beträgt:&%y{{playerBalance}}%w Rubine.&Bis später!",
                      "Voici vos rubis.&Votre nouveau solde est :&%y{{playerBalance}}%w rubis.&À plus tard !"));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BANKER_DEPOSIT_AMOUNT,
        CustomMessage("\x08Select Deposit Amount:&     rupees.&Set the amount with \xAA and&press \x9F to decide.\x0A",
                    "\x08Wähle den Einzahlungsbetrag:&     Rubine.&Stelle den Betrag mit \xAA ein und&drücke \x9F, um zu entscheiden.\x0A",
                    "\x08Sélectionnez le montant du dépôt :&     rubis.&Réglez le montant avec \xAA et&appuyez sur \x9F pour décider.\x0A"));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BANKER_DEPOSIT_CONFIRM,
        CustomMessage("You've deposited your rupees.&Your new balance is:&%y{{playerBalance}}%w rupees.&See you later!",
                    "Du hast deine Rubine eingezahlt.&Dein neues Guthaben beträgt:&%y{{playerBalance}}%w Rubine.&Bis später!",
                    "Vous avez déposé vos rubis.&Votre nouveau solde est :&%y{{playerBalance}}%w rubis.&À plus tard !"));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BANKER_REWARD_WARP_TRANSFER_INTRO,
        CustomMessage("Ho ho! Your savings have&reached %y200%w rupees!&A fine achievement, young&spender!",
                    "Ho ho! Deine Ersparnisse haben&%y200%w Rubine erreicht!&Eine feine Leistung, junger&Spender!",
                    "Ho ho ! Tes économies ont&atteint %y200%w rubis !&Une belle réussite, jeune&dépensier !"));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BANKER_REWARD_WARP_TRANSFER_ITEM,
        CustomMessage("To reward your thriftiness,&I've got a special trinket for&you... The Pirate's Charm!",
                    "Um deine Sparsamkeit zu&belohnen, habe ich ein&besonderes Schmuckstück für&dich... Den Piratenanhänger!",
                    "Pour récompenser ton&économie, j'ai un objet spécial&pour toi... Le Charme du Pirate !"));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BANKER_REWARD_WARP_TRANSFER_LORE_1,
        CustomMessage("This charm, infused with pirate magic,&can warp your treasures directly&to the bank! No more heavy&wallets for you!",
                    "Dieser Anhänger, durchtränkt mit&Piratenmagie, kann deine Schätze&direkt zur Bank warpen! Keine&schweren Geldbeutel mehr für dich!",
                    "Ce charme, imprégné de magie&pirate, peut transférer tes trésors&directement à la banque ! Fini les&bourses lourdes pour toi !"));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BANKER_REWARD_WARP_TRANSFER_LORE_2,
        CustomMessage("And that's not all! It also lets me&whisper your new balance to you&through the winds, once a warp&transfer is complete.",
                    "Und das ist noch nicht alles! Es&erlaubt mir auch, dir dein neues&Guthaben zuzuflüstern, sobald eine&Warpenübertragung abgeschlossen ist.",
                    "Et ce n'est pas tout ! Il me permet&également de te chuchoter ton nouveau&solde à travers les vents, une fois le&transfert par warp terminé."));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BANKER_REWARD_WARP_TRANSFER_LORE_3,
        CustomMessage("I've taken to calling it 'warp&transfer'. A fitting name, don't&you think? It's quite the&convenience!",
                    "Ich habe es 'Warpenübertragung'&genannt. Ein passender Name,&findest du nicht? Sehr praktisch!",
                    "Je l'ai baptisé 'transfert par warp'.&Un nom approprié, tu ne trouves&pas ? C'est très pratique !"));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BANKER_REWARD_INTEREST,
        CustomMessage("And look at that! Over %y1,000%w&rupees! Here's %y5%w more for&your dedication to saving!",
                    "Und sieh dir das an! Über %y1.000%w&Rubine! Hier sind %y5%w mehr für&deine Sparbemühungen!",
                    "Et regarde ça ! Plus de %y1 000%w&rubis ! Voici %y5%w de plus pour&ton dévouement à l'épargne !"));
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BANKER_REWARD_PIECE_OF_HEART,
        CustomMessage("Astounding! A whopping %y5,000%w&rupees saved! For such a feat,&a Piece of Heart is yours!",
                    "Erstaunlich! Ganze %y5.000%w Rubine&gespart! Für ein solches Kunststück&ist ein Herzteil dein!",
                    "Stupéfiant ! La bagatelle de %y5 000%w&rubis épargnés ! Pour un tel exploit,&un Morceau de cœur est à toi !"));
}
