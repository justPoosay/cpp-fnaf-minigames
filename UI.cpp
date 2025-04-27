//#include "UI.h"
//#include "Constants.h"
//#include <algorithm> // Dla std::min
//
//void InitUI(UIData& ui, const Texture2D& texOff, const Texture2D& texOn)
//{
//    ui.buttonVoicesOffTexture = texOff;
//    ui.buttonVoicesOnTexture = texOn;
//    ui.currentButtonTexture = ui.buttonVoicesOffTexture; // Domy�lnie "off"
//
//    const float buttonTextureWidth = (float)ui.buttonVoicesOffTexture.width;
//    const float buttonTextureHeight = (float)ui.buttonVoicesOffTexture.height;
//
//    ui.buttonPos = {
//        BUTTON_PADDING,
//        (float)VIRTUAL_SCREEN_HEIGHT - buttonTextureHeight - BUTTON_PADDING
//    };
//    ui.buttonRect = { ui.buttonPos.x, ui.buttonPos.y, buttonTextureWidth, buttonTextureHeight };
//    ui.isButtonClicked = false;
//}
//
//void UpdateUI(UIData& ui, Vector2 virtualMousePos)
//{
//    // Obs�uga Klikni�cia Przycisku - zmiana stanu tylko przy klikni�ciu
//    // Reset stanu przycisku powinien by� obs�ugiwany przez logik� gry, je�li potrzebne
//    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) // U�ywamy standardowego przycisku myszy
//    {
//        if (CheckCollisionPointRec(virtualMousePos, ui.buttonRect))
//        {
//            // Prze��cz stan i tekstur�
//            ui.isButtonClicked = !ui.isButtonClicked;
//            ui.currentButtonTexture = ui.isButtonClicked ? ui.buttonVoicesOnTexture : ui.buttonVoicesOffTexture;
//
//            // Tutaj mo�esz doda� logik�, kt�ra ma si� wydarzy� PO klikni�ciu
//            // np. wyciszenie/w��czenie d�wi�k�w (wymaga interakcji z Audio modu�em)
//            TraceLog(LOG_INFO, "UI: Przycisk klikni�ty, stan: %s", ui.isButtonClicked ? "ON" : "OFF");
//        }
//    }
//}
//
//void DrawUI(const UIData& ui)
//{
//    // Rysuj Przycisk
//    if (ui.currentButtonTexture.id > 0) {
//        DrawTextureV(ui.currentButtonTexture, ui.buttonPos, WHITE);
//    }
//}
//
//void UnloadUITextures(UIData& ui)
//{
//    // Tekstury �adowane w Game, zwalniane w Game::Shutdown
//    // UnloadTexture(ui.buttonVoicesOffTexture);
//    // UnloadTexture(ui.buttonVoicesOnTexture);
//}