#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Image.hpp>

#include <filesystem>
#include <iostream>
#include <string_view>
#include <vector>

int main() {
    const std::filesystem::path root{OBJECT_PROJECT_SOURCE_DIR};
    const std::vector<std::string_view> paths{
        "assets/pvz/ui/main_menu.png",
        "assets/pvz/ui/reference/main_menu.png",
        "assets/pvz/ui/reference/adventure_cn.png",
        "assets/pvz/ui/reference/adventure_cn_hover.png",
        "assets/pvz/ui/reference/almanac_panel.png",
        "assets/pvz/ui/reference/chooser_background.png",
        "assets/pvz/ui/reference/shovel_slot.png",
        "assets/pvz/ui/reference/shovel.png",
        "assets/pvz/ui/reference/wood_button.png",
        "assets/pvz/ui/reference/zombie_note_1.png",
        "assets/pvz/ui/reference/zombie_note_2.png",
        "assets/pvz/ui/reference/zombie_note_3.png",
        "assets/pvz/ui/pvz_logo_.png",
        "assets/pvz/ui/selectorscreen_adventure_button.png",
        "assets/pvz/ui/selectorscreen_almanac.png",
        "assets/pvz/ui/selectorscreen_options1.png",
        "assets/pvz/ui/selectorscreen_quit1.png",
        "assets/pvz/ui/lawn_mower.png",
        "assets/pvz/ui/main_menu/adventure.png",
        "assets/pvz/ui/main_menu/puzzle.png",
        "assets/pvz/ui/main_menu/minigames.png",
        "assets/pvz/ui/main_menu/almanac.png",
        "assets/pvz/ui/main_menu/welcome_sign.png",
        "assets/pvz/ui/main_menu/user_sign.png",
        "assets/pvz/ui/main_menu/trophy.png",
        "assets/pvz/ui/screens/help.png",
        "assets/pvz/ui/screens/options_menu.png",
        "assets/pvz/ui/screens/stone_button.png",
        "assets/pvz/ui/screens/small_button.png",
        "assets/pvz/ui/screens/almanac_index.jpg",
        "assets/pvz/ui/screens/award_screen.jpg",
        "assets/pvz/ui/screens/challenge_background.jpg",
        "assets/pvz/ui/screens/zombies_won.png",
        "assets/pvz/backgrounds/night_lawn.jpg",
        "assets/pvz/plants/sunflower.png",
        "assets/pvz/plants/peashooter.png",
        "assets/pvz/plants/wallnut.png",
        "assets/pvz/plants/repeater.png",
        "assets/pvz/plants/threepeater_head.png",
        "assets/pvz/plants/snowpea.png",
        "assets/pvz/plants/tallnut.gif",
        "assets/pvz/plants/potatomine_body.png",
        "assets/pvz/plants/cherrybomb.png",
        "assets/pvz/plants/torchwood_body.png",
        "assets/pvz/plants/torchwood_fire.png",
        "assets/pvz/zombies/normal.png",
        "assets/pvz/zombies/conehead.png",
        "assets/pvz/zombies/buckethead.png",
        "assets/pvz/zombies/flag.png",
        "assets/pvz/zombies/pole_vaulting.gif",
        "assets/pvz/zombies/newspaper.gif",
        "assets/pvz/projectiles/pea.png",
        "assets/pvz/projectiles/snow_pea.png",
        "assets/pvz/animations/plants/sunflower/0.png",
        "assets/pvz/animations/plants/sunflower/17.png",
        "assets/pvz/animations/plants/peashooter/0.png",
        "assets/pvz/animations/plants/peashooter/12.png",
        "assets/pvz/animations/plants/wallnut/0.png",
        "assets/pvz/animations/plants/wallnut/15.png",
        "assets/pvz/animations/plants/repeater/14.png",
        "assets/pvz/animations/plants/threepeater/15.png",
        "assets/pvz/animations/plants/threepeater/0.png",
        "assets/pvz/animations/plants/snowpea/0.png",
        "assets/pvz/animations/plants/snowpea/14.png",
        "assets/pvz/animations/plants/tallnut/13.png",
        "assets/pvz/animations/plants/tallnut/0.png",
        "assets/pvz/animations/plants/potatomine/7.png",
        "assets/pvz/animations/plants/potatomine/0.png",
        "assets/pvz/animations/plants/cherrybomb/0.png",
        "assets/pvz/animations/plants/cherrybomb/6.png",
        "assets/pvz/animations/plants/torchwood/8.png",
        "assets/pvz/animations/plants/puffshroom/0.png",
        "assets/pvz/animations/plants/puffshroom/13.png",
        "assets/pvz/animations/zombies/normal/0.png",
        "assets/pvz/animations/zombies/normal/21.png",
        "assets/pvz/animations/zombies/conehead/0.png",
        "assets/pvz/animations/zombies/conehead/20.png",
        "assets/pvz/animations/zombies/buckethead/0.png",
        "assets/pvz/animations/zombies/buckethead/14.png",
        "assets/pvz/animations/zombies/flag/11.png",
        "assets/pvz/animations/zombies/pole_vaulting/9.png",
        "assets/pvz/animations/zombies/pole_walking/24.png",
        "assets/pvz/animations/zombies/newspaper/18.png",
        "assets/pvz/animations/zombies/newspaper/0.png",
        "assets/pvz/animations/zombies/newspaper_enraged/13.png",
        "assets/pvz/animations/zombies/attack/normal/0.png",
        "assets/pvz/animations/zombies/attack/normal/20.png",
        "assets/pvz/animations/zombies/attack/conehead/10.png",
        "assets/pvz/animations/zombies/attack/buckethead/10.png",
        "assets/pvz/animations/zombies/attack/flag/10.png",
        "assets/pvz/animations/zombies/attack/pole_vaulting/13.png",
        "assets/pvz/animations/zombies/attack/newspaper/7.png",
        "assets/pvz/animations/zombies/attack/newspaper_enraged/6.png",
        "assets/pvz/animations/zombies/pole_jump/16.png",
        "assets/pvz/animations/zombies/newspaper_transition/10.png",
        "assets/pvz/animations/zombies/death/normal/9.png",
        "assets/pvz/animations/zombies/death/pole_vaulting/8.png",
        "assets/pvz/animations/zombies/death/newspaper/10.png",
        "assets/pvz/animations/effects/cherry_explosion/0.png",
        "assets/pvz/animations/effects/cherry_explosion/12.png",
        "assets/pvz/animations/effects/potato_explosion/0.png",
        "assets/pvz/animations/effects/potato_explosion/1.png",
        "assets/pvz/animations/projectiles/spore/0.png",
        "assets/pvz/animations/projectiles/spore/5.png",
        "assets/pvz/animations/projectiles/spore_hit/0.png",
        "assets/pvz/cards/card_puffshroom.png",
        "assets/pvz/minigames/challenge_background.jpg",
        "assets/pvz/minigames/scary_pot.png",
        "assets/pvz/minigames/brain.png",
        "assets/pvz/minigames/wallnut_roll.gif",
        "assets/pvz/minigames/boom_wallnut_roll.gif",
    };
    const std::vector<std::string_view> audioPaths{
        "assets/pvz/audio/UraniwaNi.ogg",
        "assets/pvz/audio/Mountains.ogg",
        "assets/pvz/audio/buttonclick.ogg",
        "assets/pvz/audio/points.ogg",
        "assets/pvz/audio/plant1.ogg",
        "assets/pvz/audio/shoop.ogg",
        "assets/pvz/audio/splat1.ogg",
        "assets/pvz/audio/hugewave.ogg",
        "assets/pvz/audio/readysetplant.ogg",
        "assets/pvz/audio/cherrybomb.ogg",
        "assets/pvz/audio/potato_mine.ogg",
        "assets/pvz/audio/puff.ogg",
        "assets/pvz/audio/lawnmower.ogg",
        "assets/pvz/audio/polevault.ogg",
        "assets/pvz/audio/newspaper_rarrgh2.ogg",
        "assets/pvz/audio/chomp.ogg",
        "assets/pvz/audio/firepea.ogg",
        "assets/pvz/audio/winmusic.ogg",
        "assets/pvz/audio/losemusic.ogg",
        "assets/pvz/audio/bowling.ogg",
        "assets/pvz/audio/bowlingimpact.ogg",
        "assets/pvz/audio/bowlingimpact2.ogg",
    };

    bool passed = true;
    for (const std::string_view relativePath : paths) {
        sf::Image image;
        if (!image.loadFromFile(root / relativePath)) {
            std::cerr << "[FAIL] could not load " << relativePath << '\n';
            passed = false;
        }
    }
    for (const std::string_view relativePath : audioPaths) {
        sf::SoundBuffer audio;
        if (!audio.loadFromFile(root / relativePath)) {
            std::cerr << "[FAIL] could not load " << relativePath << '\n';
            passed = false;
        }
    }

    if (passed) {
        std::cout << "[PASS] loaded " << paths.size() << " visual assets and "
                  << audioPaths.size() << " audio assets\n";
    }
    return passed ? 0 : 1;
}
