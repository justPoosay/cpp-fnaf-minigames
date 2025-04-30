//#include "UI.h"
//#include "Constants.h"
//#include <algorithm> // Dla std::min
//
//void InitUI(UIData& ui, const Texture2D& texOff, const Texture2D& texOn)
//{
//    ui.buttonVoicesOffTexture = texOff;
//    ui.buttonVoicesOnTexture = texOn;
//    ui.currentButtonTexture = ui.buttonVoicesOffTexture; // Domyœlnie "off"
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
//    // Obs³uga Klikniêcia Przycisku - zmiana stanu tylko przy klikniêciu
//    // Reset stanu przycisku powinien byæ obs³ugiwany przez logikê gry, jeœli potrzebne
//    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) // U¿ywamy standardowego przycisku myszy
//    {
//        if (CheckCollisionPointRec(virtualMousePos, ui.buttonRect))
//        {
//            // Prze³¹cz stan i teksturê
//            ui.isButtonClicked = !ui.isButtonClicked;
//            ui.currentButtonTexture = ui.isButtonClicked ? ui.buttonVoicesOnTexture : ui.buttonVoicesOffTexture;
//
//            // Tutaj mo¿esz dodaæ logikê, która ma siê wydarzyæ PO klikniêciu
//            // np. wyciszenie/w³¹czenie dŸwiêków (wymaga interakcji z Audio modu³em)
//            TraceLog(LOG_INFO, "UI: Przycisk klikniêty, stan: %s", ui.isButtonClicked ? "ON" : "OFF");
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
//    // Tekstury ³adowane w Game, zwalniane w Game::Shutdown
//    // UnloadTexture(ui.buttonVoicesOffTexture);
//    // UnloadTexture(ui.buttonVoicesOnTexture);
//}