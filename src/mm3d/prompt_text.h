// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <array>
#include <string>
#include <string_view>
namespace MM3D::PromptText {
struct Entry {std::string_view english,chinese,french,spanish;};
inline constexpr std::array Entries{
    Entry{"Change tab","切换菜单","Changer d'onglet","Cambiar pestaña"},
    Entry{"Select","选择","Choisir","Elegir"},
    Entry{"Choose","选择","Choisir","Elegir"}, Entry{"Return","返回","Retour","Volver"},
    Entry{"Equip to I","装备到I","Assigner I","Asignar a I"}, Entry{"Equip to II","装备到II","Assigner II","Asignar a II"},
    Entry{"Items","道具","Inventaire","Inventario"}, Entry{"Map","地图","Carte","Mapa"},
    Entry{"Zoom in","详细","Zoom +","Acercar"}, Entry{"Zoom out","全体","Zoom -","Alejar"},
    Entry{"View schedule","计划表","Voir horaires","Ver horarios"}, Entry{"View events","事件","Voir l'historique","Ver eventos"},
    Entry{"View map","地图","Carte","Mapa"}, Entry{"Set alarm","闹钟","Alarme","Alarma"},
    Entry{"Schedule details","计划表详情","Détails horaires","Detalles horario"},
    Entry{"Event details","事件详情","Détails","Detalles suceso"},
    Entry{"Songs","乐谱","Chants","Canciones"}, Entry{"Cancel","取消","Annuler","Cancelar"},
    Entry{"Move map","移动地图","Déplacer la carte","Mover mapa"},
    Entry{"Back","返回","Retour","Volver"}, Entry{"Move cursor","移动光标","Déplacer","Mover cursor"},
};
inline std::string_view Translate(unsigned language,std::string_view english) {
    for(const auto& e:Entries)if(e.english==english)
        return language==1?e.chinese:language==4?e.french:language==8?e.spanish:e.english;
    return english;
}
// Read the active message archive, not a requested language awaiting restart.
template<class M> unsigned Language(M& m) {
    const auto base=m.Read(0x6b373c);
    if(!base||*base<0x100000||*base>0x3fffffef||m.Read(*base)!=0x47534d47||m.Read(*base+4)!=1)return 2;
    return m.Read(*base+12).value_or(2);
}
template<class M> std::string_view Get(M& m,std::string_view english) {return Translate(Language(m),english);}
// Prompt literals are UTF-8; font lookup takes Unicode code points.
inline std::u32string Codes(std::string_view text) {
    std::u32string out;
    for(size_t i=0;i<text.size();) {
        unsigned c=static_cast<unsigned char>(text[i++]),n=0,min=0;
        if(c<0x80){}else if((c&0xe0)==0xc0){c&=31;n=1;min=0x80;}
        else if((c&0xf0)==0xe0){c&=15;n=2;min=0x800;}
        else if((c&0xf8)==0xf0){c&=7;n=3;min=0x10000;}else return {};
        if(i+n>text.size())return {};
        while(n--) {unsigned b=static_cast<unsigned char>(text[i++]);if((b&0xc0)!=0x80)return {};c=(c<<6)|(b&63);}
        if(c<min||c>0x10ffff||(c>=0xd800&&c<=0xdfff))return {};
        out.push_back(c);
    }
    return out;
}
}
