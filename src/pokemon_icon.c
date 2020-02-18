#include "global.h"
#include "graphics.h"
#include "mail.h"
#include "palette.h"
#include "pokemon_icon.h"
#include "sprite.h"
#include "constants/species.h"

#define POKE_ICON_BASE_PAL_TAG 56000

struct MonIconSpriteTemplate
{
    const struct OamData *oam;
    const u8 *image;
    const union AnimCmd *const *anims;
    const union AffineAnimCmd *const *affineAnims;
    void (*callback)(struct Sprite *);
    u16 paletteTag;
};

// static functions
static u8 CreateMonIconSprite(struct MonIconSpriteTemplate *, s16, s16, u8);

// .rodata

const u8 *const gMonIconTable[] =
{
    [SPECIES_NONE] = gMonIcon_Bulbasaur,
    [SPECIES_BULBASAUR] = gMonIcon_Bulbasaur,
    [SPECIES_IVYSAUR] = gMonIcon_Ivysaur,
    [SPECIES_VENUSAUR] = gMonIcon_Venusaur,
    [SPECIES_CHARMANDER] = gMonIcon_Charmander,
    [SPECIES_CHARMELEON] = gMonIcon_Charmeleon,
    [SPECIES_CHARIZARD] = gMonIcon_Charizard,
    [SPECIES_SQUIRTLE] = gMonIcon_Squirtle,
    [SPECIES_WARTORTLE] = gMonIcon_Wartortle,
    [SPECIES_BLASTOISE] = gMonIcon_Blastoise,
    [SPECIES_SNIVY] = gMonIcon_Snivy,
    [SPECIES_SERVINE] = gMonIcon_Servine,
    [SPECIES_SERPERIOR] = gMonIcon_Serperior,
    [SPECIES_WEEDLE] = gMonIcon_Weedle,
    [SPECIES_KAKUNA] = gMonIcon_Kakuna,
    [SPECIES_BEEDRILL] = gMonIcon_Beedrill,
    [SPECIES_PIDGEY] = gMonIcon_Pidgey,
    [SPECIES_PIDGEOTTO] = gMonIcon_Pidgeotto,
    [SPECIES_PIDGEOT] = gMonIcon_Pidgeot,
    [SPECIES_VULPIX_A] = gMonIcon_Alolan_Vulpix,
    [SPECIES_NINETALES_A] = gMonIcon_Alolan_Ninetales,
    [SPECIES_NOIBAT] = gMonIcon_Noibat,
    [SPECIES_NOIVERN] = gMonIcon_Noivern,
    [SPECIES_EKANS] = gMonIcon_Ekans,
    [SPECIES_ARBOK] = gMonIcon_Arbok,
    [SPECIES_PIKACHU] = gMonIcon_Pikachu,
    [SPECIES_RAICHU] = gMonIcon_Raichu,
    [SPECIES_SANDSHREW] = gMonIcon_Sandshrew,
    [SPECIES_SANDSLASH] = gMonIcon_Sandslash,
    [SPECIES_NIDORAN_F] = gMonIcon_NidoranF,
    [SPECIES_NIDORINA] = gMonIcon_Nidorina,
    [SPECIES_NIDOQUEEN] = gMonIcon_Nidoqueen,
    [SPECIES_NIDORAN_M] = gMonIcon_NidoranM,
    [SPECIES_NIDORINO] = gMonIcon_Nidorino,
    [SPECIES_NIDOKING] = gMonIcon_Nidoking,
    [SPECIES_CLEFAIRY] = gMonIcon_Clefairy,
    [SPECIES_CLEFABLE] = gMonIcon_Clefable,
    [SPECIES_VULPIX] = gMonIcon_Vulpix,
    [SPECIES_NINETALES] = gMonIcon_Ninetales,
    [SPECIES_FROAKIE] = gMonIcon_Froakie,
    [SPECIES_FROGADIER] = gMonIcon_Frogadier,
    [SPECIES_GRENINJA] = gMonIcon_Greninja,
    [SPECIES_BONDED_GRENINJA] = gMonIcon_Bonded_Greninja,
    [SPECIES_ROWLET] = gMonIcon_Rowlet,
    [SPECIES_DARTRIX] = gMonIcon_Dartrix,
    [SPECIES_DECIDUEYE] = gMonIcon_Decidueye,
    [SPECIES_BIDOOF] = gMonIcon_Bidoof,
    [SPECIES_BIBAREL] = gMonIcon_Bibarel,
    [SPECIES_VENONAT] = gMonIcon_Venonat,
    [SPECIES_VENOMOTH] = gMonIcon_Venomoth,
    [SPECIES_KARRABLAST] = gMonIcon_Karrablast,
    [SPECIES_ESCAVALIER] = gMonIcon_Escavalier,
    [SPECIES_TYRUNT] = gMonIcon_Tyrunt,
    [SPECIES_TYRANTRUM] = gMonIcon_Tyrantrum,
    [SPECIES_COMBEE] = gMonIcon_Combee,
    [SPECIES_VESPIQUEN] = gMonIcon_Vespiquen,
    [SPECIES_PURRLOIN] = gMonIcon_Purrloin,
    [SPECIES_LIEPARD] = gMonIcon_Liepard,
    [SPECIES_GROWLITHE] = gMonIcon_Growlithe,
    [SPECIES_ARCANINE] = gMonIcon_Arcanine,
    [SPECIES_PIDOVE] = gMonIcon_Pidove,
    [SPECIES_TRANQUILL] = gMonIcon_Tranquill,
    [SPECIES_UNFEZANT] = gMonIcon_Unfezant,
    [SPECIES_ABRA] = gMonIcon_Abra,
    [SPECIES_KADABRA] = gMonIcon_Kadabra,
    [SPECIES_ALAKAZAM] = gMonIcon_Alakazam,
    [SPECIES_SCATTERBUG] = gMonIcon_Scatterbug,
    [SPECIES_SPEWPA] = gMonIcon_Spewpa,
    [SPECIES_VIVILLON] = gMonIcon_Vivillon,
    [SPECIES_VENIPEDE] = gMonIcon_Venipede,
    [SPECIES_WHIRLIPEDE] = gMonIcon_Whirlipede,
    [SPECIES_SCOLIPEDE] = gMonIcon_Scolipede,
    [SPECIES_TENTACOOL] = gMonIcon_Tentacool,
    [SPECIES_TENTACRUEL] = gMonIcon_Tentacruel,
    [SPECIES_ESPURR] = gMonIcon_Espurr,
    [SPECIES_MEOWSTIC_M] = gMonIcon_MeowsticM,
    [SPECIES_MEOWSTIC_F] = gMonIcon_MeowsticF,
    [SPECIES_PONYTA] = gMonIcon_Ponyta,
    [SPECIES_RAPIDASH] = gMonIcon_Rapidash,
    [SPECIES_VULLABY] = gMonIcon_Vullaby,
    [SPECIES_MANDIBUZZ] = gMonIcon_Mandibuzz,
    [SPECIES_MAGNEMITE] = gMonIcon_Magnemite,
    [SPECIES_MAGNETON] = gMonIcon_Magneton,
    [SPECIES_MAGNEZONE] = gMonIcon_Magnezone,
    [SPECIES_ROTOM] = gMonIcon_Rotom,
    [SPECIES_LILLIPUP] = gMonIcon_Lillipup,
    [SPECIES_HERDIER] = gMonIcon_Herdier,
    [SPECIES_STOUTLAND] = gMonIcon_Stoutland,
    [SPECIES_MUDBRAY] = gMonIcon_Mudbray,
    [SPECIES_MUDSDALE] = gMonIcon_Mudsdale,
    [SPECIES_DUCKLETT] = gMonIcon_Ducklett,
    [SPECIES_SWANNA] = gMonIcon_Swanna,
    [SPECIES_GASTLY] = gMonIcon_Gastly,
    [SPECIES_HAUNTER] = gMonIcon_Haunter,
    [SPECIES_GENGAR] = gMonIcon_Gengar,
    [SPECIES_ONIX] = gMonIcon_Onix,
    [SPECIES_RUFFLET] = gMonIcon_Rufflet,
    [SPECIES_BRAVIARY] = gMonIcon_Braviary,
    [SPECIES_KRABBY] = gMonIcon_Krabby,
    [SPECIES_KINGLER] = gMonIcon_Kingler,
    [SPECIES_SCRAGGY] = gMonIcon_Scraggy,
    [SPECIES_SCRAFTY] = gMonIcon_Scrafty,
    [SPECIES_CUBCHOO] = gMonIcon_Cubchoo,
    [SPECIES_BEARTIC] = gMonIcon_Beartic,
    [SPECIES_CUBONE] = gMonIcon_Cubone,
    [SPECIES_MAROWAK] = gMonIcon_Marowak,
    [SPECIES_HITMONLEE] = gMonIcon_Hitmonlee,
    [SPECIES_HITMONCHAN] = gMonIcon_Hitmonchan,
    [SPECIES_BUNNELBY] = gMonIcon_Bunnelby,
    [SPECIES_DIGGERSBY] = gMonIcon_Diggersby,
    [SPECIES_BONDED_BEARTIC] = gMonIcon_Bonded_Beartic,
    [SPECIES_RHYHORN] = gMonIcon_Rhyhorn,
    [SPECIES_RHYDON] = gMonIcon_Rhydon,
    [SPECIES_CHANSEY] = gMonIcon_Chansey,
    [SPECIES_TANGELA] = gMonIcon_Tangela,
    [SPECIES_TANGROWTH] = gMonIcon_Tangrowth,
    [SPECIES_BUIZEL] = gMonIcon_Buizel,
    [SPECIES_FLOATZEL] = gMonIcon_Floatzel,
    [SPECIES_DRILLBUR] = gMonIcon_Drillbur,
    [SPECIES_EXCADRILL] = gMonIcon_Excadrill,
    [SPECIES_STARYU] = gMonIcon_Staryu,
    [SPECIES_STARMIE] = gMonIcon_Starmie,
    [SPECIES_BONDED_FLYGON] = gMonIcon_Bonded_Flygon,
    [SPECIES_SCYTHER] = gMonIcon_Scyther,
    [SPECIES_LARVESTA] = gMonIcon_Larvesta,
    [SPECIES_VOLCARONA] = gMonIcon_Volcarona,
    [SPECIES_FOONGUS] = gMonIcon_Foongus,
    [SPECIES_PINSIR] = gMonIcon_Pinsir,
    [SPECIES_AMOONGUSS] = gMonIcon_Amoonguss,
    [SPECIES_MAGIKARP] = gMonIcon_Magikarp,
    [SPECIES_GYARADOS] = gMonIcon_Gyarados,
    [SPECIES_LAPRAS] = gMonIcon_Lapras,
    [SPECIES_DITTO] = gMonIcon_Ditto,
    [SPECIES_EEVEE] = gMonIcon_Eevee,
    [SPECIES_VAPOREON] = gMonIcon_Vaporeon,
    [SPECIES_JOLTEON] = gMonIcon_Jolteon,
    [SPECIES_FLAREON] = gMonIcon_Flareon,
    [SPECIES_PORYGON] = gMonIcon_Porygon,
    [SPECIES_SKORUPI] = gMonIcon_Skorupi,
    [SPECIES_DRAPION] = gMonIcon_Drapion,
    [SPECIES_SKRELP] = gMonIcon_Skrelp,
    [SPECIES_DRAGALGE] = gMonIcon_Dragalge,
    [SPECIES_AERODACTYL] = gMonIcon_Aerodactyl,
    [SPECIES_SNORLAX] = gMonIcon_Snorlax,
    [SPECIES_BONDED_KYOGRE] = gMonIcon_Bonded_Kyogre,
    [SPECIES_BONDED_GROUDON] = gMonIcon_Bonded_Groudon,
    [SPECIES_BONDED_RAYQUAZA] = gMonIcon_Bonded_Rayquaza,
    [SPECIES_GOOMY] = gMonIcon_Goomy,
    [SPECIES_SLIGGOO] = gMonIcon_Sliggoo,
    [SPECIES_GOODRA] = gMonIcon_Goodra,
    [SPECIES_MEWTWO] = gMonIcon_Mewtwo,
    [SPECIES_MEW] = gMonIcon_Mew,
    [SPECIES_SNOVER] = gMonIcon_Snover,
    [SPECIES_ABOMASNOW] = gMonIcon_Abomasnow,
    [SPECIES_ABOMACIER] = gMonIcon_Abomacier,
    [SPECIES_CYNDAQUIL] = gMonIcon_Cyndaquil,
    [SPECIES_QUILAVA] = gMonIcon_Quilava,
    [SPECIES_TYPHLOSION] = gMonIcon_Typhlosion,
    [SPECIES_BASCULIN] = gMonIcon_Basculin,
    [SPECIES_WIMPOD] = gMonIcon_Wimpod,
    [SPECIES_GOLISOPOD] = gMonIcon_Golisopod,
    [SPECIES_SENTRET] = gMonIcon_Sentret,
    [SPECIES_FURRET] = gMonIcon_Furret,
    [SPECIES_FERROSEED] = gMonIcon_Ferroseed,
    [SPECIES_FERROTHORN] = gMonIcon_Ferrothorn,
    [SPECIES_CLAUNCHER] = gMonIcon_Clauncher,
    [SPECIES_CLAWITZER] = gMonIcon_Clawitzer,
    [SPECIES_SPINARAK] = gMonIcon_Spinarak,
    [SPECIES_ARIADOS] = gMonIcon_Ariados,
    [SPECIES_MIMIKYU] = gMonIcon_Mimikyu,
    [SPECIES_CHINCHOU] = gMonIcon_Chinchou,
    [SPECIES_LANTURN] = gMonIcon_Lanturn,
    [SPECIES_SEWADDLE] = gMonIcon_Sewaddle,
    [SPECIES_SWADLOON] = gMonIcon_Swadloon,
    [SPECIES_LEAVANNY] = gMonIcon_Leavanny,
    [SPECIES_TOGEPI] = gMonIcon_Togepi,
    [SPECIES_TOGETIC] = gMonIcon_Togetic,
    [SPECIES_TOGEKISS] = gMonIcon_Togekiss,
    [SPECIES_PHANTUMP] = gMonIcon_Phantump,
    [SPECIES_MAREEP] = gMonIcon_Mareep,
    [SPECIES_FLAAFFY] = gMonIcon_Flaaffy,
    [SPECIES_AMPHAROS] = gMonIcon_Ampharos,
    [SPECIES_TREVENANT] = gMonIcon_Trevenant,
    [SPECIES_MARILL] = gMonIcon_Marill,
    [SPECIES_AZUMARILL] = gMonIcon_Azumarill,
    [SPECIES_GIBLE] = gMonIcon_Gible,
    [SPECIES_GABITE] = gMonIcon_Gabite,
    [SPECIES_GARCHOMP] = gMonIcon_Garchomp,
    [SPECIES_HONEDGE] = gMonIcon_Honedge,
    [SPECIES_DOUBLADE] = gMonIcon_Doublade,
    [SPECIES_AEGISLASH] = gMonIcon_Aegislash,
    [SPECIES_SCIMITEYE] = gMonIcon_Scimiteye,
    [SPECIES_JAWGUILE] = gMonIcon_Jawguile,
    [SPECIES_BONDED_RAICHU] = gMonIcon_Bonded_Raichu,
    [SPECIES_WOOPER] = gMonIcon_Wooper,
    [SPECIES_QUAGSIRE] = gMonIcon_Quagsire,
    [SPECIES_ESPEON] = gMonIcon_Espeon,
    [SPECIES_UMBREON] = gMonIcon_Umbreon,
    [SPECIES_LEAFEON] = gMonIcon_Leafeon,
    [SPECIES_GLACEON] = gMonIcon_Glaceon,
    [SPECIES_SYLVEON] = gMonIcon_Sylveon,
    [SPECIES_FLABEBE] = gMonIcon_Flabebe,
    [SPECIES_FLOETTE] = gMonIcon_Floette,
    [SPECIES_FLORGES] = gMonIcon_Florges,
    [SPECIES_JOLTIK] = gMonIcon_Joltik,
    [SPECIES_GALVANTULA] = gMonIcon_Galvantula,
    [SPECIES_AUDINO] = gMonIcon_Audino,
    [SPECIES_WEAVILE] = gMonIcon_Weavile,
    [SPECIES_STEELIX] = gMonIcon_Steelix,
    [SPECIES_POPPLIO] = gMonIcon_Popplio,
    [SPECIES_BRIONNE] = gMonIcon_Brionne,
    [SPECIES_PRIMARINA] = gMonIcon_Primarina,
    [SPECIES_SCIZOR] = gMonIcon_Scizor,
    [SPECIES_MEDIREIGN] = gMonIcon_Medireign,
    [SPECIES_HERACROSS] = gMonIcon_Heracross,
    [SPECIES_SNEASEL] = gMonIcon_Sneasel,
    [SPECIES_TEDDIURSA] = gMonIcon_Teddiursa,
    [SPECIES_URSARING] = gMonIcon_Ursaring,
    [SPECIES_BRONZOR] = gMonIcon_Bronzor,
    [SPECIES_BRONZONG] = gMonIcon_Bronzong,
    [SPECIES_SWINUB] = gMonIcon_Swinub,
    [SPECIES_PILOSWINE] = gMonIcon_Piloswine,
    [SPECIES_MAMOSWINE] = gMonIcon_Mamoswine,
    [SPECIES_ZEKROM] = gMonIcon_Zekrom,
    [SPECIES_SHINX] = gMonIcon_Shinx,
    [SPECIES_LUXIO] = gMonIcon_Luxio,
    [SPECIES_LUXRAY] = gMonIcon_Luxray,
    [SPECIES_SKARMORY] = gMonIcon_Skarmory,
    [SPECIES_HOUNDOUR] = gMonIcon_Houndour,
    [SPECIES_HOUNDOOM] = gMonIcon_Houndoom,
    [SPECIES_HOUNDREAD] = gMonIcon_Houndread,
    [SPECIES_LITLEO] = gMonIcon_Litleo,
    [SPECIES_PYROAR] = gMonIcon_Pyroar,
    [SPECIES_PORYGON2] = gMonIcon_Porygon2,
    [SPECIES_RESHIRAM] = gMonIcon_Reshiram,
    [SPECIES_SMEARGLE] = gMonIcon_Smeargle,
    [SPECIES_TYROGUE] = gMonIcon_Tyrogue,
    [SPECIES_HITMONTOP] = gMonIcon_Hitmontop,
    [SPECIES_GOTHITA] = gMonIcon_Gothita,
    [SPECIES_GOTHORITA] = gMonIcon_Gothorita,
    [SPECIES_GOTHITELLE] = gMonIcon_Gothitelle,
    [SPECIES_BONDED_MEWTWO_Y] = gMonIcon_Bonded_Mewtwo_Y,
    [SPECIES_BLISSEY] = gMonIcon_Blissey,
    [SPECIES_ROTOM_FAN] = gMonIcon_Rotom_Fan,
    [SPECIES_ROTOM_MOW] = gMonIcon_Rotom_Mow,
    [SPECIES_ROTOM_HEAT] = gMonIcon_Rotom_Heat,
    [SPECIES_LARVITAR] = gMonIcon_Larvitar,
    [SPECIES_PUPITAR] = gMonIcon_Pupitar,
    [SPECIES_TYRANITAR] = gMonIcon_Tyranitar,
    [SPECIES_ROTOM_WASH] = gMonIcon_Rotom_Wash,
    [SPECIES_ROTOM_FREEZE] = gMonIcon_Rotom_Freeze,
    [SPECIES_CELEBI] = gMonIcon_Celebi,
    [SPECIES_TREECKO] = gMonIcon_Treecko,
    [SPECIES_GROVYLE] = gMonIcon_Grovyle,
    [SPECIES_SCEPTILE] = gMonIcon_Sceptile,
    [SPECIES_TORCHIC] = gMonIcon_Torchic,
    [SPECIES_COMBUSKEN] = gMonIcon_Combusken,
    [SPECIES_BLAZIKEN] = gMonIcon_Blaziken,
    [SPECIES_MUDKIP] = gMonIcon_Mudkip,
    [SPECIES_MARSHTOMP] = gMonIcon_Marshtomp,
    [SPECIES_SWAMPERT] = gMonIcon_Swampert,
    [SPECIES_POOCHYENA] = gMonIcon_Poochyena,
    [SPECIES_MIGHTYENA] = gMonIcon_Mightyena,
    [SPECIES_ZIGZAGOON] = gMonIcon_Zigzagoon,
    [SPECIES_LINOONE] = gMonIcon_Linoone,
    [SPECIES_PETILIL] = gMonIcon_Petilil,
    [SPECIES_LILLIGANT] = gMonIcon_Lilligant,
    [SPECIES_COTTONEE] = gMonIcon_Cottonee,
    [SPECIES_WHIMSICOTT] = gMonIcon_Whimsicott,
    [SPECIES_SOLOSIS] = gMonIcon_Solosis,
    [SPECIES_DUOSION] = gMonIcon_Duosion,
    [SPECIES_REUNICLUS] = gMonIcon_Reuniclus,
    [SPECIES_PANCHAM] = gMonIcon_Pancham,
    [SPECIES_PANGORO] = gMonIcon_Pangoro,
    [SPECIES_MIENFOO] = gMonIcon_Mienfoo,
    [SPECIES_MIENSHAO] = gMonIcon_Mienshao,
    [SPECIES_AMAURA] = gMonIcon_Amaura,
    [SPECIES_AURORUS] = gMonIcon_Aurorus,
    [SPECIES_DIALGA] = gMonIcon_Dialga,
    [SPECIES_SHELMET] = gMonIcon_Shelmet,
    [SPECIES_ACCELGOR] = gMonIcon_Accelgor,
    [SPECIES_SHROOMISH] = gMonIcon_Shroomish,
    [SPECIES_BRELOOM] = gMonIcon_Breloom,
    [SPECIES_PALKIA] = gMonIcon_Palkia,
    [SPECIES_WINGULL] = gMonIcon_Wingull,
    [SPECIES_PELIPPER] = gMonIcon_Pelipper,
    [SPECIES_RIOLU] = gMonIcon_Riolu,
    [SPECIES_LUCARIO] = gMonIcon_Lucario,
    [SPECIES_WAILMER] = gMonIcon_Wailmer,
    [SPECIES_WAILORD] = gMonIcon_Wailord,
    [SPECIES_SKITTY] = gMonIcon_Skitty,
    [SPECIES_DELCATTY] = gMonIcon_Delcatty,
    [SPECIES_ROGGENROLA] = gMonIcon_Roggenrola,
    [SPECIES_BOLDORE] = gMonIcon_Boldore,
    [SPECIES_GIGALITH] = gMonIcon_Gigalith,
    [SPECIES_SANDSHREW_A] = gMonIcon_SandshrewA,
    [SPECIES_SANDSLASH_A] = gMonIcon_SandslashA,
    [SPECIES_SABLEYE] = gMonIcon_Sableye,
    [SPECIES_LITWICK] = gMonIcon_Litwick,
    [SPECIES_LAMPENT] = gMonIcon_Lampent,
    [SPECIES_CHANDELURE] = gMonIcon_Chandelure,
    [SPECIES_CORPHISH] = gMonIcon_Corphish,
    [SPECIES_CRAWDAUNT] = gMonIcon_Crawdaunt,
    [SPECIES_FEEBAS] = gMonIcon_Feebas,
    [SPECIES_MILOTIC] = gMonIcon_Milotic,
    [SPECIES_CARVANHA] = gMonIcon_Carvanha,
    [SPECIES_SHARPEDO] = gMonIcon_Sharpedo,
    [SPECIES_TRAPINCH] = gMonIcon_Trapinch,
    [SPECIES_VIBRAVA] = gMonIcon_Vibrava,
    [SPECIES_FLYGON] = gMonIcon_Flygon,
    [SPECIES_DWEBBLE] = gMonIcon_Dwebble,
    [SPECIES_CRUSTLE] = gMonIcon_Crustle,
    [SPECIES_ELECTRIKE] = gMonIcon_Electrike,
    [SPECIES_MANECTRIC] = gMonIcon_Manectric,
    [SPECIES_MANETHUNDR] = gMonIcon_Manethundr,
    [SPECIES_PORYGONZ] = gMonIcon_PorygonZ,
    [SPECIES_TYNAMO] = gMonIcon_Tynamo,
    [SPECIES_EELEKTRIK] = gMonIcon_Eelektrik,
    [SPECIES_EELEKTROSS] = gMonIcon_Eelektross,
    [SPECIES_CACNEA] = gMonIcon_Cacnea,
    [SPECIES_CACTURNE] = gMonIcon_Cacturne,
    [SPECIES_SNORUNT] = gMonIcon_Snorunt,
    [SPECIES_GLALIE] = gMonIcon_Glalie,
    [SPECIES_FROSLASS] = gMonIcon_Froslass,
    [SPECIES_TIRTOUGA] = gMonIcon_Tirtouga,
    [SPECIES_CARRACOSTA] = gMonIcon_Carracosta,
    [SPECIES_SPOINK] = gMonIcon_Spoink,
    [SPECIES_GRUMPIG] = gMonIcon_Grumpig,
    [SPECIES_SALANDIT] = gMonIcon_Salandit,
    [SPECIES_SALAZZLE] = gMonIcon_Salazzle,
    [SPECIES_MAWILE] = gMonIcon_Mawile,
    [SPECIES_MEDITITE] = gMonIcon_Meditite,
    [SPECIES_MEDICHAM] = gMonIcon_Medicham,
    [SPECIES_SWABLU] = gMonIcon_Swablu,
    [SPECIES_ALTARIA] = gMonIcon_Altaria,
    [SPECIES_TIMBURR] = gMonIcon_Timburr,
    [SPECIES_GURDURR] = gMonIcon_Gurdurr,
    [SPECIES_CONKELDURR] = gMonIcon_Conkeldurr,
    [SPECIES_GIRATINA] = gMonIcon_Giratina,
    [SPECIES_FLETCHLING] = gMonIcon_Fletchling,
    [SPECIES_FLETCHINDR] = gMonIcon_Fletchindr,
    [SPECIES_TALONFLAME] = gMonIcon_Talonflame,
    [SPECIES_GIRATINA_ORIGIN] = gMonIcon_Giratina_Origin,
    [SPECIES_BONDED_SERPERIOR] = gMonIcon_Bonded_Serperior,
    [SPECIES_KYUREM] = gMonIcon_Kyurem,
    [SPECIES_MELOETTA] = gMonIcon_Meloetta,
    [SPECIES_BUNEARY] = gMonIcon_Buneary,
    [SPECIES_LOPUNNY] = gMonIcon_Lopunny,
    [SPECIES_AZELF] = gMonIcon_Azelf,
    [SPECIES_MESPRIT] = gMonIcon_Mesprit,
    [SPECIES_UXIE] = gMonIcon_Uxie,
    [SPECIES_ABSOL] = gMonIcon_Absol,
    [SPECIES_SHUPPET] = gMonIcon_Shuppet,
    [SPECIES_BANETTE] = gMonIcon_Banette,
    [SPECIES_BANSHEETTE] = gMonIcon_Bansheette,
    [SPECIES_ABSOLOM] = gMonIcon_Absolom,
    [SPECIES_RELICANTH] = gMonIcon_Relicanth,
    [SPECIES_ARON] = gMonIcon_Aron,
    [SPECIES_LAIRON] = gMonIcon_Lairon,
    [SPECIES_AGGRON] = gMonIcon_Aggron,
    [SPECIES_ALOLAN_MAROWAK] = gMonIcon_MarowakA,
    [SPECIES_RHYPERIOR] = gMonIcon_Rhyperior,
    [SPECIES_GALLADE] = gMonIcon_Gallade,
    [SPECIES_LILEEP] = gMonIcon_Lileep,
    [SPECIES_CRADILY] = gMonIcon_Cradily,
    [SPECIES_ANORITH] = gMonIcon_Anorith,
    [SPECIES_ARMALDO] = gMonIcon_Armaldo,
    [SPECIES_RALTS] = gMonIcon_Ralts,
    [SPECIES_KIRLIA] = gMonIcon_Kirlia,
    [SPECIES_GARDEVOIR] = gMonIcon_Gardevoir,
    [SPECIES_BAGON] = gMonIcon_Bagon,
    [SPECIES_SHELGON] = gMonIcon_Shelgon,
    [SPECIES_SALAMENCE] = gMonIcon_Salamence,
    [SPECIES_BELDUM] = gMonIcon_Beldum,
    [SPECIES_METANG] = gMonIcon_Metang,
    [SPECIES_METAGROSS] = gMonIcon_Metagross,
    [SPECIES_ROCKRUFF] = gMonIcon_Rockruff,
    [SPECIES_LYCANROC] = gMonIcon_Lycanroc,
    [SPECIES_BONDED_AERODACTYL] = gMonIcon_Bonded_Aerodactyl,
    [SPECIES_KYOGRE] = gMonIcon_Kyogre,
    [SPECIES_GROUDON] = gMonIcon_Groudon,
    [SPECIES_RAYQUAZA] = gMonIcon_Rayquaza,
    [SPECIES_KYUREM_WHITE] = gMonIcon_Kyurem_White,
    [SPECIES_KYUREM_BLACK] = gMonIcon_Kyurem_Black,
    [SPECIES_BONDED_AGGRON] = gMonIcon_Bonded_Aggron,
    [SPECIES_BONDED_ALAKAZAM] = gMonIcon_Bonded_Alakazam,
    [SPECIES_ARCEUS] = gMonIcon_Arceus,
    [SPECIES_EGG] = gMonIcon_Egg,
    [SPECIES_BONDED_ALTARIA] = gMonIcon_Bonded_Altaria,
    [SPECIES_BONDED_AMPHAROS] = gMonIcon_Bonded_Ampharos,
    [SPECIES_BONDED_AUDINO] = gMonIcon_Bonded_Audino,
    [SPECIES_BONDED_BEEDRILL] = gMonIcon_Bonded_Beedrill,
    [SPECIES_BONDED_BLASTOISE] = gMonIcon_Bonded_Blastoise,
    [SPECIES_BONDED_BLAZIKEN] = gMonIcon_Bonded_Blaziken,
    [SPECIES_BONDED_CHARIZARD] = gMonIcon_Bonded_Charizard_y,
    [SPECIES_BONDED_GALLADE] = gMonIcon_Bonded_Gallade,
    [SPECIES_BONDED_GARCHOMP] = gMonIcon_Bonded_Garchomp,
    [SPECIES_BONDED_GARDEVOIR] = gMonIcon_Bonded_Gardevoir,
    [SPECIES_BONDED_GENGAR] = gMonIcon_Bonded_Gengar,
    [SPECIES_BONDED_GLALIE] = gMonIcon_Bonded_Glalie,
    [SPECIES_BONDED_GYARADOS] = gMonIcon_Bonded_Gyarados,
    [SPECIES_BONDED_HERACROSS] = gMonIcon_Bonded_Heracross,
    [SPECIES_BONDED_LOPUNNY] = gMonIcon_Bonded_Lopunny,
    [SPECIES_BONDED_LUCARIO] = gMonIcon_Bonded_Lucario,
    [SPECIES_BONDED_METAGROSS] = gMonIcon_Bonded_Metagross,
    [SPECIES_BONDED_PIDGEOT] = gMonIcon_Bonded_Pidgeot,
    [SPECIES_BONDED_PINSIR] = gMonIcon_Bonded_Pinsir,
    [SPECIES_BONDED_MEWTWO_X] = gMonIcon_Bonded_Mewtwo_x,
    [SPECIES_BONDED_SALAMENCE] = gMonIcon_Bonded_Salamence,
    [SPECIES_BONDED_SCEPTILE] = gMonIcon_Bonded_Sceptile,
    [SPECIES_BONDED_SCIZOR] = gMonIcon_Bonded_Scizor,
    [SPECIES_BONDED_STEELIX] = gMonIcon_Bonded_Steelix,
    [SPECIES_BONDED_SWAMPERT] = gMonIcon_Bonded_Swampert,
    [SPECIES_BONDED_TYRANITAR] = gMonIcon_Bonded_Tyranitar,
    [SPECIES_BONDED_VENUSAUR] = gMonIcon_Bonded_Venusaur,
};

const u8 gMonIconPaletteIndices[] =
{
    [SPECIES_NONE] = 0,
    [SPECIES_BULBASAUR] = 4,
    [SPECIES_IVYSAUR] = 4,
    [SPECIES_VENUSAUR] = 4,
    [SPECIES_CHARMANDER] = 0,
    [SPECIES_CHARMELEON] = 0,
    [SPECIES_CHARIZARD] = 0,
    [SPECIES_SQUIRTLE] = 0,
    [SPECIES_WARTORTLE] = 2,
    [SPECIES_BLASTOISE] = 2,
    [SPECIES_SNIVY] = 1,
    [SPECIES_SERVINE] = 1,
    [SPECIES_SERPERIOR] = 1,
    [SPECIES_WEEDLE] = 2,
    [SPECIES_KAKUNA] = 2,
    [SPECIES_BEEDRILL] = 2,
    [SPECIES_PIDGEY] = 0,
    [SPECIES_PIDGEOTTO] = 0,
    [SPECIES_PIDGEOT] = 0,
    [SPECIES_VULPIX_A] = 2,
    [SPECIES_NINETALES_A] = 2,
    [SPECIES_NOIBAT] = 0,
    [SPECIES_NOIVERN] = 0,
    [SPECIES_EKANS] = 2,
    [SPECIES_ARBOK] = 2,
    [SPECIES_PIKACHU] = 2,
    [SPECIES_RAICHU] = 0,
    [SPECIES_SANDSHREW] = 2,
    [SPECIES_SANDSLASH] = 2,
    [SPECIES_NIDORAN_F] = 0,
    [SPECIES_NIDORINA] = 0,
    [SPECIES_NIDOQUEEN] = 2,
    [SPECIES_NIDORAN_M] = 2,
    [SPECIES_NIDORINO] = 2,
    [SPECIES_NIDOKING] = 2,
    [SPECIES_CLEFAIRY] = 0,
    [SPECIES_CLEFABLE] = 0,
    [SPECIES_VULPIX] = 5,
    [SPECIES_NINETALES] = 3,
    [SPECIES_FROAKIE] = 0,
    [SPECIES_FROGADIER] = 0,
    [SPECIES_GRENINJA] = 0,
    [SPECIES_BONDED_GRENINJA] = 0,
    [SPECIES_ROWLET] = 0,
    [SPECIES_DARTRIX] = 1,
    [SPECIES_DECIDUEYE] = 1,
    [SPECIES_BIDOOF] = 2,
    [SPECIES_BIBAREL] = 2,
    [SPECIES_VENONAT] = 2,
    [SPECIES_VENOMOTH] = 2,
    [SPECIES_KARRABLAST] = 0,
    [SPECIES_ESCAVALIER] = 0,
    [SPECIES_TYRUNT] = 2,
    [SPECIES_TYRANTRUM] = 0,
    [SPECIES_COMBEE] = 0,
    [SPECIES_VESPIQUEN] = 0,
    [SPECIES_PURRLOIN] = 0,
    [SPECIES_LIEPARD] = 0,
    [SPECIES_GROWLITHE] = 3,
    [SPECIES_ARCANINE] = 3,
    [SPECIES_PIDOVE] = 0,
    [SPECIES_TRANQUILL] = 0,
    [SPECIES_UNFEZANT] = 1,
    [SPECIES_ABRA] = 2,
    [SPECIES_KADABRA] = 2,
    [SPECIES_ALAKAZAM] = 2,
    [SPECIES_SCATTERBUG] = 1,
    [SPECIES_SPEWPA] = 1,
    [SPECIES_VIVILLON] = 0,
    [SPECIES_VENIPEDE] = 1,
    [SPECIES_WHIRLIPEDE] = 2,
    [SPECIES_SCOLIPEDE] = 2,
    [SPECIES_TENTACOOL] = 0,
    [SPECIES_TENTACRUEL] = 0,
    [SPECIES_ESPURR] = 2,
    [SPECIES_MEOWSTIC_M] = 0,
    [SPECIES_MEOWSTIC_F] = 0,
    [SPECIES_PONYTA] = 0,
    [SPECIES_RAPIDASH] = 0,
    [SPECIES_VULLABY] = 0,
    [SPECIES_MANDIBUZZ] = 1,
    [SPECIES_MAGNEMITE] = 0,
    [SPECIES_MAGNETON] = 0,
    [SPECIES_MAGNEZONE] = 0,
    [SPECIES_ROTOM] = 0,
    [SPECIES_LILLIPUP] = 2,
    [SPECIES_HERDIER] = 2,
    [SPECIES_STOUTLAND] = 2,
    [SPECIES_MUDBRAY] = 2,
    [SPECIES_MUDSDALE] = 0,
    [SPECIES_DUCKLETT] = 0,
    [SPECIES_SWANNA] = 2,
    [SPECIES_GASTLY] = 2,
    [SPECIES_HAUNTER] = 2,
    [SPECIES_GENGAR] = 2,
    [SPECIES_ONIX] = 2,
    [SPECIES_RUFFLET] = 2,
    [SPECIES_BRAVIARY] = 0,
    [SPECIES_KRABBY] = 0,
    [SPECIES_KINGLER] = 0,
    [SPECIES_SCRAGGY] = 2,
    [SPECIES_SCRAFTY] = 0,
    [SPECIES_CUBCHOO] = 0,
    [SPECIES_BEARTIC] = 0,
    [SPECIES_CUBONE] = 2,
    [SPECIES_MAROWAK] = 2,
    [SPECIES_HITMONLEE] = 2,
    [SPECIES_HITMONCHAN] = 2,
    [SPECIES_BUNNELBY] = 2,
    [SPECIES_DIGGERSBY] = 2,
    [SPECIES_BONDED_BEARTIC] = 0,
    [SPECIES_RHYHORN] = 1,
    [SPECIES_RHYDON] = 1,
    [SPECIES_CHANSEY] = 0,
    [SPECIES_TANGELA] = 0,
    [SPECIES_TANGROWTH] = 0,
    [SPECIES_BUIZEL] = 0,
    [SPECIES_FLOATZEL] = 0,
    [SPECIES_DRILLBUR] = 0,
    [SPECIES_EXCADRILL] = 0,
    [SPECIES_STARYU] = 2,
    [SPECIES_STARMIE] = 2,
    [SPECIES_BONDED_FLYGON] = 1,
    [SPECIES_SCYTHER] = 1,
    [SPECIES_LARVESTA] = 0,
    [SPECIES_VOLCARONA] = 0,
    [SPECIES_FOONGUS] = 0,
    [SPECIES_PINSIR] = 2,
    [SPECIES_AMOONGUSS] = 1,
    [SPECIES_MAGIKARP] = 0,
    [SPECIES_GYARADOS] = 0,
    [SPECIES_LAPRAS] = 2,
    [SPECIES_DITTO] = 2,
    [SPECIES_EEVEE] = 2,
    [SPECIES_VAPOREON] = 0,
    [SPECIES_JOLTEON] = 2,
    [SPECIES_FLAREON] = 3,
    [SPECIES_PORYGON] = 0,
    [SPECIES_SKORUPI] = 0,
    [SPECIES_DRAPION] = 2,
    [SPECIES_SKRELP] = 2,
    [SPECIES_DRAGALGE] = 5,
    [SPECIES_AERODACTYL] = 2,
    [SPECIES_SNORLAX] = 3,
    [SPECIES_BONDED_KYOGRE] = 0,
    [SPECIES_BONDED_GROUDON] = 0,
    [SPECIES_BONDED_RAYQUAZA] = 1,
    [SPECIES_GOOMY] = 5,
    [SPECIES_SLIGGOO] = 5,
    [SPECIES_GOODRA] = 5,
    [SPECIES_MEWTWO] = 2,
    [SPECIES_MEW] = 0,
    [SPECIES_SNOVER] = 1,
    [SPECIES_ABOMASNOW] = 1,
    [SPECIES_ABOMACIER] = 1,
    [SPECIES_CYNDAQUIL] = 3,
    [SPECIES_QUILAVA] = 3,
    [SPECIES_TYPHLOSION] = 3,
    [SPECIES_BASCULIN] = 1,
    [SPECIES_WIMPOD] = 2,
    [SPECIES_GOLISOPOD] = 2,
    [SPECIES_SENTRET] = 2,
    [SPECIES_FURRET] = 2,
    [SPECIES_FERROSEED] = 1,
    [SPECIES_FERROTHORN] = 1,
    [SPECIES_CLAUNCHER] = 0,
    [SPECIES_CLAWITZER] = 0,
    [SPECIES_SPINARAK] = 1,
    [SPECIES_ARIADOS] = 0,
    [SPECIES_MIMIKYU] = 1,
    [SPECIES_CHINCHOU] = 2,
    [SPECIES_LANTURN] = 0,
    [SPECIES_SEWADDLE] = 1,
    [SPECIES_SWADLOON] = 1,
    [SPECIES_LEAVANNY] = 1,
    [SPECIES_TOGEPI] = 0,
    [SPECIES_TOGETIC] = 0,
    [SPECIES_TOGEKISS] = 2,
    [SPECIES_PHANTUMP] = 1,
    [SPECIES_MAREEP] = 0,
    [SPECIES_FLAAFFY] = 0,
    [SPECIES_AMPHAROS] = 0,
    [SPECIES_TREVENANT] = 1,
    [SPECIES_MARILL] = 0,
    [SPECIES_AZUMARILL] = 0,
    [SPECIES_GIBLE] = 0,
    [SPECIES_GABITE] = 0,
    [SPECIES_GARCHOMP] = 0,
    [SPECIES_HONEDGE] = 2,
    [SPECIES_DOUBLADE] = 2,
    [SPECIES_AEGISLASH] = 2,
    [SPECIES_SCIMITEYE] = 1,
    [SPECIES_JAWGUILE] = 0,
    [SPECIES_BONDED_RAICHU] = 2,
    [SPECIES_WOOPER] = 0,
    [SPECIES_QUAGSIRE] = 0,
    [SPECIES_ESPEON] = 2,
    [SPECIES_UMBREON] = 0,
    [SPECIES_LEAFEON] = 1,
    [SPECIES_GLACEON] = 0,
    [SPECIES_SYLVEON] = 0,
    [SPECIES_FLABEBE] = 1,
    [SPECIES_FLOETTE] = 1,
    [SPECIES_FLORGES] = 0,
    [SPECIES_JOLTIK] = 0,
    [SPECIES_GALVANTULA] = 2,
    [SPECIES_AUDINO] = 1,
    [SPECIES_WEAVILE] = 0,
    [SPECIES_STEELIX] = 0,
    [SPECIES_POPPLIO] = 0,
    [SPECIES_BRIONNE] = 0,
    [SPECIES_PRIMARINA] = 0,
    [SPECIES_SCIZOR] = 0,
    [SPECIES_MEDIREIGN] = 0,
    [SPECIES_HERACROSS] = 0,
    [SPECIES_SNEASEL] = 0,
    [SPECIES_TEDDIURSA] = 0,
    [SPECIES_URSARING] = 2,
    [SPECIES_BRONZOR] = 0,
    [SPECIES_BRONZONG] = 0,
    [SPECIES_SWINUB] = 2,
    [SPECIES_PILOSWINE] = 2,
    [SPECIES_MAMOSWINE] = 2,
    [SPECIES_ZEKROM] = 2,
    [SPECIES_SHINX] = 0,
    [SPECIES_LUXIO] = 0,
    [SPECIES_LUXRAY] = 0,
    [SPECIES_SKARMORY] = 0,
    [SPECIES_HOUNDOUR] = 0,
    [SPECIES_HOUNDOOM] = 0,
    [SPECIES_HOUNDREAD] = 0,
    [SPECIES_LITLEO] = 2,
    [SPECIES_PYROAR] = 2,
    [SPECIES_PORYGON2] = 0,
    [SPECIES_RESHIRAM] = 0,
    [SPECIES_SMEARGLE] = 1,
    [SPECIES_TYROGUE] = 2,
    [SPECIES_HITMONTOP] = 2,
    [SPECIES_GOTHITA] = 2,
    [SPECIES_GOTHORITA] = 2,
    [SPECIES_GOTHITELLE] = 2,
    [SPECIES_BONDED_MEWTWO_Y] = 2,
    [SPECIES_BLISSEY] = 0,
    [SPECIES_ROTOM_FAN] = 0,
    [SPECIES_ROTOM_MOW] = 0,
    [SPECIES_ROTOM_HEAT] = 0,
    [SPECIES_LARVITAR] = 1,
    [SPECIES_PUPITAR] = 2,
    [SPECIES_TYRANITAR] = 4,
    [SPECIES_ROTOM_WASH] = 0,
    [SPECIES_ROTOM_FREEZE] = 5,
    [SPECIES_CELEBI] = 1,
    [SPECIES_TREECKO] = 1,
    [SPECIES_GROVYLE] = 0,
    [SPECIES_SCEPTILE] = 1,
    [SPECIES_TORCHIC] = 0,
    [SPECIES_COMBUSKEN] = 0,
    [SPECIES_BLAZIKEN] = 0,
    [SPECIES_MUDKIP] = 0,
    [SPECIES_MARSHTOMP] = 0,
    [SPECIES_SWAMPERT] = 0,
    [SPECIES_POOCHYENA] = 2,
    [SPECIES_MIGHTYENA] = 2,
    [SPECIES_ZIGZAGOON] = 2,
    [SPECIES_LINOONE] = 2,
    [SPECIES_PETILIL] = 1,
    [SPECIES_LILLIGANT] = 1,
    [SPECIES_COTTONEE] = 1,
    [SPECIES_WHIMSICOTT] = 1,
    [SPECIES_SOLOSIS] = 1,
    [SPECIES_DUOSION] = 1,
    [SPECIES_REUNICLUS] = 1,
    [SPECIES_PANCHAM] = 1,
    [SPECIES_PANGORO] = 1,
    [SPECIES_MIENFOO] = 1,
    [SPECIES_MIENSHAO] = 2,
    [SPECIES_AMAURA] = 0,
    [SPECIES_AURORUS] = 0,
    [SPECIES_DIALGA] = 2,
    [SPECIES_SHELMET] = 1,
    [SPECIES_ACCELGOR] = 1,
    [SPECIES_SHROOMISH] = 1,
    [SPECIES_BRELOOM] = 1,
    [SPECIES_PALKIA] = 2,
    [SPECIES_WINGULL] = 0,
    [SPECIES_PELIPPER] = 0,
    [SPECIES_RIOLU] = 2,
    [SPECIES_LUCARIO] = 2,
    [SPECIES_WAILMER] = 2,
    [SPECIES_WAILORD] = 0,
    [SPECIES_SKITTY] = 0,
    [SPECIES_DELCATTY] = 2,
    [SPECIES_ROGGENROLA] = 2,
    [SPECIES_BOLDORE] = 0,
    [SPECIES_GIGALITH] = 0,
    [SPECIES_SANDSHREW_A] = 0,
    [SPECIES_SANDSLASH_A] = 0,
    [SPECIES_SABLEYE] = 2,
    [SPECIES_LITWICK] = 2,
    [SPECIES_LAMPENT] = 2,
    [SPECIES_CHANDELURE] = 2,
    [SPECIES_CORPHISH] = 0,
    [SPECIES_CRAWDAUNT] = 0,
    [SPECIES_FEEBAS] = 2,
    [SPECIES_MILOTIC] = 2,
    [SPECIES_CARVANHA] = 0,
    [SPECIES_SHARPEDO] = 0,
    [SPECIES_TRAPINCH] = 0,
    [SPECIES_VIBRAVA] = 1,
    [SPECIES_FLYGON] = 1,
    [SPECIES_DWEBBLE] = 0,
    [SPECIES_CRUSTLE] = 2,
    [SPECIES_ELECTRIKE] = 1,
    [SPECIES_MANECTRIC] = 0,
    [SPECIES_MANETHUNDR] = 0,
    [SPECIES_PORYGONZ] = 0,
    [SPECIES_TYNAMO] = 0,
    [SPECIES_EELEKTRIK] = 0,
    [SPECIES_EELEKTROSS] = 0,
    [SPECIES_CACNEA] = 1,
    [SPECIES_CACTURNE] = 1,
    [SPECIES_SNORUNT] = 2,
    [SPECIES_GLALIE] = 0,
    [SPECIES_FROSLASS] = 0,
    [SPECIES_TIRTOUGA] = 2,
    [SPECIES_CARRACOSTA] = 2,
    [SPECIES_SPOINK] = 0,
    [SPECIES_GRUMPIG] = 2,
    [SPECIES_SALANDIT] = 2,
    [SPECIES_SALAZZLE] = 0,
    [SPECIES_MAWILE] = 2,
    [SPECIES_MEDITITE] = 0,
    [SPECIES_MEDICHAM] = 0,
    [SPECIES_SWABLU] = 0,
    [SPECIES_ALTARIA] = 0,
    [SPECIES_TIMBURR] = 1,
    [SPECIES_GURDURR] = 1,
    [SPECIES_CONKELDURR] = 1,
    [SPECIES_GIRATINA] = 0,
    [SPECIES_FLETCHLING] = 2,
    [SPECIES_FLETCHINDR] = 2,
    [SPECIES_TALONFLAME] = 2,
    [SPECIES_GIRATINA_ORIGIN] = 0,
    [SPECIES_BONDED_SERPERIOR] = 1,
    [SPECIES_KYUREM] = 0,
    [SPECIES_MELOETTA] = 4,
    [SPECIES_BUNEARY] = 2,
    [SPECIES_LOPUNNY] = 2,
    [SPECIES_AZELF] = 0,
    [SPECIES_MESPRIT] = 0,
    [SPECIES_UXIE] = 0,
    [SPECIES_ABSOL] = 0,
    [SPECIES_SHUPPET] = 0,
    [SPECIES_BANETTE] = 0,
    [SPECIES_BANSHEETTE] = 0,
    [SPECIES_ABSOLOM] = 0,
    [SPECIES_RELICANTH] = 2,
    [SPECIES_ARON] = 2,
    [SPECIES_LAIRON] = 2,
    [SPECIES_AGGRON] = 2,
    [SPECIES_ALOLAN_MAROWAK] = 1,
    [SPECIES_RHYPERIOR] = 0,
    [SPECIES_GALLADE] = 1,
    [SPECIES_LILEEP] = 2,
    [SPECIES_CRADILY] = 1,
    [SPECIES_ANORITH] = 0,
    [SPECIES_ARMALDO] = 2,
    [SPECIES_RALTS] = 1,
    [SPECIES_KIRLIA] = 1,
    [SPECIES_GARDEVOIR] = 1,
    [SPECIES_BAGON] = 0,
    [SPECIES_SHELGON] = 2,
    [SPECIES_SALAMENCE] = 0,
    [SPECIES_BELDUM] = 0,
    [SPECIES_METANG] = 0,
    [SPECIES_METAGROSS] = 0,
    [SPECIES_ROCKRUFF] = 2,
    [SPECIES_LYCANROC] = 2,
    [SPECIES_BONDED_AERODACTYL] = 2,
    [SPECIES_KYOGRE] = 2,
    [SPECIES_GROUDON] = 0,
    [SPECIES_RAYQUAZA] = 1,
    [SPECIES_KYUREM_WHITE] = 0,
    [SPECIES_KYUREM_BLACK] = 0,
    [SPECIES_BONDED_AGGRON] = 2,
    [SPECIES_BONDED_ALAKAZAM] = 2,
    [SPECIES_ARCEUS] = 1,
    [SPECIES_EGG] = 1,
    [SPECIES_BONDED_ALTARIA] = 0,
    [SPECIES_BONDED_AMPHAROS] = 0,
    [SPECIES_BONDED_AUDINO] = 1,
    [SPECIES_BONDED_BEEDRILL] = 2,
    [SPECIES_BONDED_BLASTOISE] = 2,
    [SPECIES_BONDED_BLAZIKEN] = 0,
    [SPECIES_BONDED_CHARIZARD] = 0,
    [SPECIES_BONDED_GALLADE] = 1,
    [SPECIES_BONDED_GARCHOMP] = 0,
    [SPECIES_BONDED_GARDEVOIR] = 1,
    [SPECIES_BONDED_GENGAR] = 2,
    [SPECIES_BONDED_GLALIE] = 0,
    [SPECIES_BONDED_GYARADOS] = 0,
    [SPECIES_BONDED_HERACROSS] = 0,
    [SPECIES_BONDED_LOPUNNY] = 2,
    [SPECIES_BONDED_LUCARIO] = 2,
    [SPECIES_BONDED_METAGROSS] = 0,
    [SPECIES_BONDED_PIDGEOT] = 0,
    [SPECIES_BONDED_PINSIR] = 2,
    [SPECIES_BONDED_MEWTWO_X] = 2,
    [SPECIES_BONDED_SALAMENCE] = 0,
    [SPECIES_BONDED_SCEPTILE] = 1,
    [SPECIES_BONDED_SCIZOR] = 0,
    [SPECIES_BONDED_STEELIX] = 0,
    [SPECIES_BONDED_SWAMPERT] = 0,
    [SPECIES_BONDED_TYRANITAR] = 1,
    [SPECIES_BONDED_VENUSAUR] = 1,
};

const struct SpritePalette gMonIconPaletteTable[] =
{
    { gMonIconPalettes[0], POKE_ICON_BASE_PAL_TAG + 0 },
    { gMonIconPalettes[1], POKE_ICON_BASE_PAL_TAG + 1 },
    { gMonIconPalettes[2], POKE_ICON_BASE_PAL_TAG + 2 },
    { gMonIconPalettes[3], POKE_ICON_BASE_PAL_TAG + 3 },
    { gMonIconPalettes[4], POKE_ICON_BASE_PAL_TAG + 4 },
    { gMonIconPalettes[5], POKE_ICON_BASE_PAL_TAG + 5 },
};

const struct OamData sMonIconOamData =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .x = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
};

// fastest to slowest

static const union AnimCmd sAnim_0[] =
{
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(1, 6),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_1[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_2[] =
{
    ANIMCMD_FRAME(0, 14),
    ANIMCMD_FRAME(1, 14),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_3[] =
{
    ANIMCMD_FRAME(0, 22),
    ANIMCMD_FRAME(1, 22),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_4[] =
{
    ANIMCMD_FRAME(0, 29),
    ANIMCMD_FRAME(0, 29), // frame 0 is repeated
    ANIMCMD_JUMP(0),
};

const union AnimCmd *const sMonIconAnims[] =
{
    sAnim_0,
    sAnim_1,
    sAnim_2,
    sAnim_3,
    sAnim_4,
};

static const union AffineAnimCmd sAffineAnim_0[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 0, 10),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_1[] =
{
    AFFINEANIMCMD_FRAME(-2, -2, 0, 122),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const sMonIconAffineAnims[] =
{
    sAffineAnim_0,
    sAffineAnim_1,
};

const u16 sSpriteImageSizes[3][4] =
{
    [ST_OAM_SQUARE] = 
    {
        [SPRITE_SIZE(8x8)]   =  0x20,
        [SPRITE_SIZE(16x16)] =  0x80,
        [SPRITE_SIZE(32x32)] = 0x200,
        [SPRITE_SIZE(64x64)] = 0x800,
    },
    [ST_OAM_H_RECTANGLE] = 
    {
        [SPRITE_SIZE(16x8)]  =  0x40,
        [SPRITE_SIZE(32x8)]  =  0x80,
        [SPRITE_SIZE(32x16)] = 0x100,
        [SPRITE_SIZE(64x32)] = 0x400,
    },
    [ST_OAM_V_RECTANGLE] = 
    {
        [SPRITE_SIZE(8x16)]  =  0x40,
        [SPRITE_SIZE(8x32)]  =  0x80,
        [SPRITE_SIZE(16x32)] = 0x100,
        [SPRITE_SIZE(32x64)] = 0x400,
    },
};

u8 CreateMonIcon(u16 species, void (*callback)(struct Sprite *), s16 x, s16 y, u8 subpriority, u32 personality, bool32 handleBonded_Alakazam)
{
    u8 spriteId;
    struct MonIconSpriteTemplate iconTemplate =
    {
        .oam = &sMonIconOamData,
        .image = GetMonIconPtr(species, personality, handleBonded_Alakazam),
        .anims = sMonIconAnims,
        .affineAnims = sMonIconAffineAnims,
        .callback = callback,
        .paletteTag = POKE_ICON_BASE_PAL_TAG + gMonIconPaletteIndices[species],
    };

    if (species > NUM_SPECIES)
        iconTemplate.paletteTag = POKE_ICON_BASE_PAL_TAG;

    spriteId = CreateMonIconSprite(&iconTemplate, x, y, subpriority);

    UpdateMonIconFrame(&gSprites[spriteId]);

    return spriteId;
}

u8 sub_80D2D78(u16 species, void (*callback)(struct Sprite *), s16 x, s16 y, u8 subpriority, bool32 extra)
{
    u8 spriteId;
    struct MonIconSpriteTemplate iconTemplate =
    {
        .oam = &sMonIconOamData,
        .image = NULL,
        .anims = sMonIconAnims,
        .affineAnims = sMonIconAffineAnims,
        .callback = callback,
        .paletteTag = POKE_ICON_BASE_PAL_TAG + gMonIconPaletteIndices[species],
    };

    iconTemplate.image = GetMonIconTiles(species, extra);
    spriteId = CreateMonIconSprite(&iconTemplate, x, y, subpriority);

    UpdateMonIconFrame(&gSprites[spriteId]);

    return spriteId;
}

u16 GetIconSpecies(u16 species, u32 personality)
{
    u16 result;

    if (species == SPECIES_FLABEBE)
    {
        u16 letter = GetUnownLetterByPersonality(personality);
        if (letter == 0)
            letter = SPECIES_FLABEBE;
        else
            letter += (SPECIES_BONDED_ALTARIA - 1);
        result = letter;
    }
    else
    {
        if (species > NUM_SPECIES)
            result = 260;
        else
            result = species;
    }

    return result;
}

u16 GetUnownLetterByPersonality(u32 personality)
{
    if (!personality)
        return 0;
    else
        return (((personality & 0x3000000) >> 18) | ((personality & 0x30000) >> 12) | ((personality & 0x300) >> 6) | (personality & 0x3)) % 0x1C;
}

u16 sub_80D2E84(u16 species)
{
    u16 value;

    if (MailSpeciesToSpecies(species, &value) == SPECIES_FLABEBE)
    {
        value += SPECIES_FLABEBE;
        return value;
    }
    else
    {
        if (species > NUM_SPECIES)
            species = 0;// That's an oddly specific species.

        return GetIconSpecies(species, 0);
    }
}

const u8 *GetMonIconPtr(u16 species, u32 personality, bool32 handleBonded_Alakazam)
{
    return GetMonIconTiles(GetIconSpecies(species, personality), handleBonded_Alakazam);
}

void FreeAndDestroyMonIconSprite(struct Sprite *sprite)
{
    sub_80D328C(sprite);
}

void LoadMonIconPalettes(void)
{
    u8 i;
    for (i = 0; i < ARRAY_COUNT(gMonIconPaletteTable); i++)
        LoadSpritePalette(&gMonIconPaletteTable[i]);
}

// unused
void SafeLoadMonIconPalette(u16 species)
{
    u8 palIndex;
    if (species > NUM_SPECIES)
        species = 260;
    palIndex = gMonIconPaletteIndices[species];
    if (IndexOfSpritePaletteTag(gMonIconPaletteTable[palIndex].tag) == 0xFF)
        LoadSpritePalette(&gMonIconPaletteTable[palIndex]);
}

void LoadMonIconPalette(u16 species)
{
    u8 palIndex = gMonIconPaletteIndices[species];
    if (IndexOfSpritePaletteTag(gMonIconPaletteTable[palIndex].tag) == 0xFF)
        LoadSpritePalette(&gMonIconPaletteTable[palIndex]);
}

void FreeMonIconPalettes(void)
{
    u8 i;
    for (i = 0; i < 6; i++)
        FreeSpritePaletteByTag(gMonIconPaletteTable[i].tag);
}

// unused
void SafeFreeMonIconPalette(u16 species)
{
    u8 palIndex;
    if (species > NUM_SPECIES)
        species = 260;
    palIndex = gMonIconPaletteIndices[species];
    FreeSpritePaletteByTag(gMonIconPaletteTable[palIndex].tag);
}

void FreeMonIconPalette(u16 species)
{
    u8 palIndex;
    palIndex = gMonIconPaletteIndices[species];
    FreeSpritePaletteByTag(gMonIconPaletteTable[palIndex].tag);
}

void SpriteCB_MonIcon(struct Sprite *sprite)
{
    UpdateMonIconFrame(sprite);
}

const u8* GetMonIconTiles(u16 species, bool32 handleBonded_Alakazam)
{
    const u8* iconSprite = gMonIconTable[species];
    if (species == SPECIES_BONDED_ALAKAZAM && handleBonded_Alakazam == TRUE)
    {
        iconSprite = (const u8*)(0x400 + (u32)iconSprite); // use the specific Bonded_Alakazam form icon (Speed in this case)
    }
    return iconSprite;
}

void sub_80D304C(u16 offset)
{
    s32 i;
    const struct SpritePalette* monIconPalettePtr;

    if (offset <= 0xA0)
    {
        monIconPalettePtr = gMonIconPaletteTable;
        for(i = 5; i >= 0; i--)
        {
            LoadPalette(monIconPalettePtr->data, offset, 0x20);
            offset += 0x10;
            monIconPalettePtr++;
        }
    }
}

u8 GetValidMonIconPalIndex(u16 species)
{
    if (species > NUM_SPECIES)
        species = 260;
    return gMonIconPaletteIndices[species];
}

u8 GetMonIconPaletteIndexFromSpecies(u16 species)
{
    return gMonIconPaletteIndices[species];
}

const u16* GetValidMonIconPalettePtr(u16 species)
{
    if (species > NUM_SPECIES)
        species = 260;
    return gMonIconPaletteTable[gMonIconPaletteIndices[species]].data;
}

u8 UpdateMonIconFrame(struct Sprite *sprite)
{
    u8 result = 0;

    if (sprite->animDelayCounter == 0)
    {
        s16 frame = sprite->anims[sprite->animNum][sprite->animCmdIndex].frame.imageValue;

        switch (frame)
        {
        case -1:
            break;
        case -2:
            sprite->animCmdIndex = 0;
            break;
        default:
            RequestSpriteCopy(
                // pointer arithmetic is needed to get the correct pointer to perform the sprite copy on.
                // because sprite->images is a struct def, it has to be casted to (u8 *) before any
                // arithmetic can be performed.
                (u8 *)sprite->images + (sSpriteImageSizes[sprite->oam.shape][sprite->oam.size] * frame),
                (u8 *)(OBJ_VRAM0 + sprite->oam.tileNum * TILE_SIZE_4BPP),
                sSpriteImageSizes[sprite->oam.shape][sprite->oam.size]);
            sprite->animDelayCounter = sprite->anims[sprite->animNum][sprite->animCmdIndex].frame.duration & 0xFF;
            sprite->animCmdIndex++;
            result = sprite->animCmdIndex;
            break;
        }
    }
    else
    {
        sprite->animDelayCounter--;
    }
    return result;
}

static u8 CreateMonIconSprite(struct MonIconSpriteTemplate *iconTemplate, s16 x, s16 y, u8 subpriority)
{
    u8 spriteId;

    struct SpriteFrameImage image = { NULL, sSpriteImageSizes[iconTemplate->oam->shape][iconTemplate->oam->size] };

    struct SpriteTemplate spriteTemplate =
    {
        .tileTag = 0xFFFF,
        .paletteTag = iconTemplate->paletteTag,
        .oam = iconTemplate->oam,
        .anims = iconTemplate->anims,
        .images = &image,
        .affineAnims = iconTemplate->affineAnims,
        .callback = iconTemplate->callback,
    };

    spriteId = CreateSprite(&spriteTemplate, x, y, subpriority);
    gSprites[spriteId].animPaused = TRUE;
    gSprites[spriteId].animBeginning = FALSE;
    gSprites[spriteId].images = (const struct SpriteFrameImage *)iconTemplate->image;
    return spriteId;
}

void sub_80D328C(struct Sprite *sprite)
{
    struct SpriteFrameImage image = { NULL, sSpriteImageSizes[sprite->oam.shape][sprite->oam.size] };
    sprite->images = &image;
    DestroySprite(sprite);
}

void SetPartyHPBarSprite(struct Sprite *sprite, u8 animNum)
{
    sprite->animNum = animNum;
    sprite->animDelayCounter = 0;
    sprite->animCmdIndex = 0;
}
