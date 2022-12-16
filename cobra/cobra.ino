#include <FastLED.h>

/**********************
 * TODO:
 * Splash Screen
 * Improve snake waving animation
 * Make apple animation
 * Make snake bite animation
 * Bug: head color is blue when eating and size is less than 8
 * Bug: after Game Over should reset top animation
 * Bug: after Game Over sometimes the new snake goes in wrong direction
 * Implement Button Interruption instead of loop for reading button state
 * More maps
 *********************/
 
FASTLED_USING_NAMESPACE
 
#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif
 
#define DATA_PIN    3        //pino do arduino conectado a fita de LEDS WS2812B
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB      //ordem das cores na fita de led WS2812B
#define NUM_LEDS    600      //numero de leds na fita
#define RIGHT_BTN   4
#define LEFT_BTN    5
#define M           30       //numero de linhas do Monitor
#define N           20       //numero de colunas do Monitor
#define PLAYM       20       //numero de linhas da PlayArea
#define PLAYN       20       //numero de colunas da PlayArea
#define TOPM        8        //numero de linhas da TopArea
#define TOPN        20       //numero de colunas da TopArea
#define AREASIZE    (PLAYM*PLAYN)
#define SIZE0       4
#define X0          5
#define Y0          6
#define HEADING0    1
#define SNAKECOLOR  0x000050
#define HEADCOLOR   0x204000
#define FOODCOLOR   0x500000
#define MAPCOLOR    0x050805
#define PERIOD0     200
#define MINPERIOD   50
#define TAILSIZE    8
#define MAP         1

CRGB leds[NUM_LEDS];
DEFINE_GRADIENT_PALETTE( blueGp ) {
  //0,    30,10,0,
  0,    0,  0,  0,
  8,    0,  0, 80,
 255,   0,  0, 80,
};
CRGBPalette16 bluePalette = blueGp;

unsigned long period = PERIOD0;
unsigned long topPeriod = 0;
unsigned long startTopMillis;
unsigned long currentTopMillis;
unsigned long startMillis;
unsigned long currentMillis;
__uint24 playArea[PLAYN][PLAYM] = { 0x000000 };
__uint24 topArea[TOPN][TOPM] = { 0x000000 };
__uint24 tailColor[TAILSIZE] = { 0x000000 };
bool rightPressed = false;
bool leftPressed = false;
byte topAnimation = 0;

short px2strip(byte x, byte y){ //converte coordenadas xy para endereço do monitor (0 a 599)
  return x*M + y + (x%2)*(M-1-2*y);
}

void strip2px(short stripPos, byte &x, byte &y){ //converte coordenadas xy para endereço do monitor (0 a 599)
  x = stripPos/M;
  y = stripPos%M;
  if(x%2){
    y = M - y -1;
  }
}

void playArea2px(byte x, byte y){
  leds[px2strip(x, y)] = playArea[x][y];
}

void topArea2px(byte x, byte y){
  leds[px2strip(x, y+21)] = topArea[x][y];
}

void plotPlayArea(){
  //Serial.println("plotPlayArea");
  for(byte x=0; x<PLAYN; x++){
    for(byte y=0; y<PLAYM; y++){
      playArea2px(x, y);
    }
  }
  FastLED.show();
}

void plotTopArea(){
  //Serial.println("plotPlayArea");
  for(byte x=0; x<TOPN; x++){
    for(byte y=0; y<TOPM; y++){
      topArea2px(x, y);
    }
  }
  FastLED.show();
}

void clearPlayArea(){
  for(byte x=0; x<PLAYN; x++){
    for(byte y=0; y<PLAYM; y++){
      playArea[x][y]=0;
    }
  }
}

void clearTopArea(){
  for(byte x=0; x<TOPN; x++){
    for(byte y=0; y<TOPM; y++){
      topArea[x][y]=0;
    }
  }
}

void plotBordas(){
  leds[29] = 0x121212;leds[30] = 0x121212;leds[89] = 0x121212;leds[90] = 0x121212;leds[149] = 0x121212;leds[150] = 0x121212;leds[209] = 0x121212;leds[210] = 0x121212;leds[269] = 0x121212;leds[270] = 0x121212;leds[329] = 0x121212;leds[330] = 0x121212;leds[389] = 0x121212;leds[390] = 0x121212;leds[449] = 0x121212;leds[450] = 0x121212;leds[509] = 0x121212;leds[510] = 0x121212;leds[569] = 0x121212;leds[570] = 0x121212;
  for(byte i=0; i<=PLAYN; i++){
    if(PLAYM < M && i < N){
      leds[px2strip(i, PLAYM)] = 0x121212;
    }
  }
  for(byte j=0; j<=PLAYM; j++){
    if(PLAYN < N && j < M){
      leds[px2strip(PLAYN, j)] = 0x121212;
    }
  }
}

void plotCobra(){
  leds[28] = 0x000000;leds[31] = 0x000000;leds[88] = 0x000000;leds[91] = 0x000000;leds[148] = 0x000000;leds[151] = 0x000000;leds[208] = 0x000000;leds[211] = 0x000000;leds[268] = 0x000000;leds[271] = 0x000000;leds[328] = 0x000000;leds[331] = 0x000000;leds[388] = 0x000000;leds[391] = 0x000000;leds[448] = 0x000000;leds[451] = 0x000000;leds[508] = 0x000000;leds[511] = 0x000000;leds[568] = 0x000000;leds[571] = 0x000000;
  leds[27] = 0x000000;leds[32] = 0x000000;leds[87] = 0x000000;leds[92] = 0x000000;leds[147] = 0x000000;leds[152] = 0x000000;leds[207] = 0x000000;leds[212] = 0x000000;leds[267] = 0x000000;leds[272] = 0x000000;leds[327] = 0x000000;leds[332] = 0x000000;leds[387] = 0x000000;leds[392] = 0x000000;leds[447] = 0x000000;leds[452] = 0x000000;leds[507] = 0x000000;leds[512] = 0x000000;leds[567] = 0x000000;leds[572] = 0x000000;
  leds[26] = 0x000000;leds[33] = 0x121212;leds[86] = 0x121212;leds[93] = 0x121212;leds[146] = 0x000000;leds[153] = 0x000000;leds[206] = 0x000000;leds[213] = 0x000000;leds[266] = 0x000000;leds[273] = 0x121212;leds[326] = 0x000000;leds[333] = 0x000000;leds[386] = 0x000000;leds[393] = 0x000000;leds[446] = 0x000000;leds[453] = 0x000000;leds[506] = 0x121212;leds[513] = 0x121212;leds[566] = 0x121212;leds[573] = 0x000000;
  leds[25] = 0x000000;leds[34] = 0x121212;leds[85] = 0x000000;leds[94] = 0x000000;leds[145] = 0x000000;leds[154] = 0x121212;leds[205] = 0x121212;leds[214] = 0x121212;leds[265] = 0x000000;leds[274] = 0x121212;leds[325] = 0x121212;leds[334] = 0x121212;leds[385] = 0x000000;leds[394] = 0x121212;leds[445] = 0x121212;leds[454] = 0x000000;leds[505] = 0x121212;leds[514] = 0x000000;leds[565] = 0x121212;leds[574] = 0x000000;
  leds[24] = 0x000000;leds[35] = 0x121212;leds[84] = 0x000000;leds[95] = 0x000000;leds[144] = 0x000000;leds[155] = 0x121212;leds[204] = 0x000000;leds[215] = 0x121212;leds[264] = 0x000000;leds[275] = 0x121212;leds[324] = 0x000000;leds[335] = 0x121212;leds[384] = 0x000000;leds[395] = 0x121212;leds[444] = 0x000000;leds[455] = 0x000000;leds[504] = 0x121212;leds[515] = 0x121212;leds[564] = 0x121212;leds[575] = 0x000000;
  leds[23] = 0x000000;leds[36] = 0x121212;leds[83] = 0x121212;leds[96] = 0x121212;leds[143] = 0x000000;leds[156] = 0x121212;leds[203] = 0x121212;leds[216] = 0x121212;leds[263] = 0x000000;leds[276] = 0x121212;leds[323] = 0x121212;leds[336] = 0x121212;leds[383] = 0x000000;leds[396] = 0x121212;leds[443] = 0x000000;leds[456] = 0x000000;leds[503] = 0x121212;leds[516] = 0x000000;leds[563] = 0x121212;leds[576] = 0x000000;
  leds[22] = 0x000000;leds[37] = 0x000000;leds[82] = 0x000000;leds[97] = 0x000000;leds[142] = 0x000000;leds[157] = 0x000000;leds[202] = 0x000000;leds[217] = 0x000000;leds[262] = 0x000000;leds[277] = 0x000000;leds[322] = 0x000000;leds[337] = 0x000000;leds[382] = 0x000000;leds[397] = 0x000000;leds[442] = 0x000000;leds[457] = 0x000000;leds[502] = 0x000000;leds[517] = 0x000000;leds[562] = 0x000000;leds[577] = 0x000000;
  leds[21] = 0x000000;leds[38] = 0x000000;leds[81] = 0x000000;leds[98] = 0x000000;leds[141] = 0x000000;leds[158] = 0x000000;leds[201] = 0x000000;leds[218] = 0x000000;leds[261] = 0x000000;leds[278] = 0x000000;leds[321] = 0x000000;leds[338] = 0x000000;leds[381] = 0x000000;leds[398] = 0x000000;leds[441] = 0x000000;leds[458] = 0x000000;leds[501] = 0x000000;leds[518] = 0x000000;leds[561] = 0x000000;leds[578] = 0x000000;
}

void plotGameOver(){
  leds[19] = 0x000000;leds[40] = 0x000000;leds[79] = 0x000000;leds[100] = 0x000000;leds[139] = 0x000000;leds[160] = 0x000000;leds[199] = 0x000000;leds[220] = 0x000000;leds[259] = 0x000000;leds[280] = 0x000000;leds[319] = 0x000000;leds[340] = 0x000000;leds[379] = 0x000000;leds[400] = 0x000000;leds[439] = 0x000000;leds[460] = 0x000000;leds[499] = 0x000000;leds[520] = 0x000000;leds[559] = 0x000000;leds[580] = 0x000000;
  leds[18] = 0x000000;leds[41] = 0x000000;leds[78] = 0x000000;leds[101] = 0x000000;leds[138] = 0x000000;leds[161] = 0x000000;leds[198] = 0x000000;leds[221] = 0x000000;leds[258] = 0x000000;leds[281] = 0x000000;leds[318] = 0x000000;leds[341] = 0x000000;leds[378] = 0x000000;leds[401] = 0x000000;leds[438] = 0x000000;leds[461] = 0x000000;leds[498] = 0x000000;leds[521] = 0x000000;leds[558] = 0x000000;leds[581] = 0x000000;
  leds[17] = 0x000000;leds[42] = 0x000000;leds[77] = 0x000000;leds[102] = 0x000000;leds[137] = 0x000000;leds[162] = 0x000000;leds[197] = 0x000000;leds[222] = 0x000000;leds[257] = 0x000000;leds[282] = 0x000000;leds[317] = 0x000000;leds[342] = 0x000000;leds[377] = 0x000000;leds[402] = 0x000000;leds[437] = 0x000000;leds[462] = 0x000000;leds[497] = 0x000000;leds[522] = 0x000000;leds[557] = 0x000000;leds[582] = 0x000000;
  leds[16] = 0x000000;leds[43] = 0x121212;leds[76] = 0x121212;leds[103] = 0x121212;leds[136] = 0x121212;leds[163] = 0x000000;leds[196] = 0x121212;leds[223] = 0x121212;leds[256] = 0x121212;leds[283] = 0x000000;leds[316] = 0x121212;leds[343] = 0x121212;leds[376] = 0x121212;leds[403] = 0x121212;leds[436] = 0x121212;leds[463] = 0x000000;leds[496] = 0x121212;leds[523] = 0x121212;leds[556] = 0x121212;leds[583] = 0x000000;
  leds[15] = 0x000000;leds[44] = 0x121212;leds[75] = 0x000000;leds[104] = 0x000000;leds[135] = 0x000000;leds[164] = 0x000000;leds[195] = 0x121212;leds[224] = 0x000000;leds[255] = 0x121212;leds[284] = 0x000000;leds[315] = 0x121212;leds[344] = 0x000000;leds[375] = 0x121212;leds[404] = 0x000000;leds[435] = 0x121212;leds[464] = 0x000000;leds[495] = 0x121212;leds[524] = 0x000000;leds[555] = 0x000000;leds[584] = 0x000000;
  leds[14] = 0x000000;leds[45] = 0x121212;leds[74] = 0x000000;leds[105] = 0x121212;leds[134] = 0x121212;leds[165] = 0x000000;leds[194] = 0x121212;leds[225] = 0x121212;leds[254] = 0x121212;leds[285] = 0x000000;leds[314] = 0x121212;leds[345] = 0x000000;leds[374] = 0x121212;leds[405] = 0x000000;leds[434] = 0x121212;leds[465] = 0x000000;leds[494] = 0x121212;leds[525] = 0x121212;leds[554] = 0x121212;leds[585] = 0x000000;
  leds[13] = 0x000000;leds[46] = 0x121212;leds[73] = 0x000000;leds[106] = 0x000000;leds[133] = 0x121212;leds[166] = 0x000000;leds[193] = 0x121212;leds[226] = 0x000000;leds[253] = 0x121212;leds[286] = 0x000000;leds[313] = 0x121212;leds[346] = 0x000000;leds[373] = 0x121212;leds[406] = 0x000000;leds[433] = 0x121212;leds[466] = 0x000000;leds[493] = 0x121212;leds[526] = 0x000000;leds[553] = 0x000000;leds[586] = 0x000000;
  leds[12] = 0x000000;leds[47] = 0x121212;leds[72] = 0x121212;leds[107] = 0x121212;leds[132] = 0x121212;leds[167] = 0x000000;leds[192] = 0x121212;leds[227] = 0x000000;leds[252] = 0x121212;leds[287] = 0x000000;leds[312] = 0x121212;leds[347] = 0x000000;leds[372] = 0x121212;leds[407] = 0x000000;leds[432] = 0x121212;leds[467] = 0x000000;leds[492] = 0x121212;leds[527] = 0x121212;leds[552] = 0x121212;leds[587] = 0x000000;
  leds[11] = 0x000000;leds[48] = 0x000000;leds[71] = 0x000000;leds[108] = 0x000000;leds[131] = 0x000000;leds[168] = 0x000000;leds[191] = 0x000000;leds[228] = 0x000000;leds[251] = 0x000000;leds[288] = 0x000000;leds[311] = 0x000000;leds[348] = 0x000000;leds[371] = 0x000000;leds[408] = 0x000000;leds[431] = 0x000000;leds[468] = 0x000000;leds[491] = 0x000000;leds[528] = 0x000000;leds[551] = 0x000000;leds[588] = 0x000000;
  leds[10] = 0x000000;leds[49] = 0x000000;leds[70] = 0x000000;leds[109] = 0x000000;leds[130] = 0x000000;leds[169] = 0x000000;leds[190] = 0x000000;leds[229] = 0x000000;leds[250] = 0x000000;leds[289] = 0x000000;leds[310] = 0x000000;leds[349] = 0x000000;leds[370] = 0x000000;leds[409] = 0x000000;leds[430] = 0x000000;leds[469] = 0x000000;leds[490] = 0x000000;leds[529] = 0x000000;leds[550] = 0x000000;leds[589] = 0x000000;
  leds[9] = 0x000000;leds[50] = 0x000000;leds[69] = 0x121212;leds[110] = 0x121212;leds[129] = 0x121212;leds[170] = 0x121212;leds[189] = 0x000000;leds[230] = 0x121212;leds[249] = 0x000000;leds[290] = 0x121212;leds[309] = 0x000000;leds[350] = 0x121212;leds[369] = 0x121212;leds[410] = 0x121212;leds[429] = 0x000000;leds[470] = 0x121212;leds[489] = 0x121212;leds[530] = 0x121212;leds[549] = 0x000000;leds[590] = 0x000000;
  leds[8] = 0x000000;leds[51] = 0x000000;leds[68] = 0x121212;leds[111] = 0x000000;leds[128] = 0x000000;leds[171] = 0x121212;leds[188] = 0x000000;leds[231] = 0x121212;leds[248] = 0x000000;leds[291] = 0x121212;leds[308] = 0x000000;leds[351] = 0x121212;leds[368] = 0x000000;leds[411] = 0x000000;leds[428] = 0x000000;leds[471] = 0x121212;leds[488] = 0x000000;leds[531] = 0x121212;leds[548] = 0x000000;leds[591] = 0x000000;
  leds[7] = 0x000000;leds[52] = 0x000000;leds[67] = 0x121212;leds[112] = 0x000000;leds[127] = 0x000000;leds[172] = 0x121212;leds[187] = 0x000000;leds[232] = 0x121212;leds[247] = 0x000000;leds[292] = 0x121212;leds[307] = 0x000000;leds[352] = 0x121212;leds[367] = 0x121212;leds[412] = 0x121212;leds[427] = 0x000000;leds[472] = 0x121212;leds[487] = 0x121212;leds[532] = 0x000000;leds[547] = 0x000000;leds[592] = 0x000000;
  leds[6] = 0x000000;leds[53] = 0x000000;leds[66] = 0x121212;leds[113] = 0x000000;leds[126] = 0x000000;leds[173] = 0x121212;leds[186] = 0x000000;leds[233] = 0x121212;leds[246] = 0x000000;leds[293] = 0x121212;leds[306] = 0x000000;leds[353] = 0x121212;leds[366] = 0x000000;leds[413] = 0x000000;leds[426] = 0x000000;leds[473] = 0x121212;leds[486] = 0x000000;leds[533] = 0x121212;leds[546] = 0x000000;leds[593] = 0x000000;
  leds[5] = 0x000000;leds[54] = 0x000000;leds[65] = 0x121212;leds[114] = 0x121212;leds[125] = 0x121212;leds[174] = 0x121212;leds[185] = 0x000000;leds[234] = 0x000000;leds[245] = 0x121212;leds[294] = 0x000000;leds[305] = 0x000000;leds[354] = 0x121212;leds[365] = 0x121212;leds[414] = 0x121212;leds[425] = 0x000000;leds[474] = 0x121212;leds[485] = 0x000000;leds[534] = 0x121212;leds[545] = 0x000000;leds[594] = 0x000000;
  leds[4] = 0x000000;leds[55] = 0x000000;leds[64] = 0x000000;leds[115] = 0x000000;leds[124] = 0x000000;leds[175] = 0x000000;leds[184] = 0x000000;leds[235] = 0x000000;leds[244] = 0x000000;leds[295] = 0x000000;leds[304] = 0x000000;leds[355] = 0x000000;leds[364] = 0x000000;leds[415] = 0x000000;leds[424] = 0x000000;leds[475] = 0x000000;leds[484] = 0x000000;leds[535] = 0x000000;leds[544] = 0x000000;leds[595] = 0x000000;
  leds[3] = 0x000000;leds[56] = 0x000000;leds[63] = 0x000000;leds[116] = 0x000000;leds[123] = 0x000000;leds[176] = 0x000000;leds[183] = 0x000000;leds[236] = 0x000000;leds[243] = 0x000000;leds[296] = 0x000000;leds[303] = 0x000000;leds[356] = 0x000000;leds[363] = 0x000000;leds[416] = 0x000000;leds[423] = 0x000000;leds[476] = 0x000000;leds[483] = 0x000000;leds[536] = 0x000000;leds[543] = 0x000000;leds[596] = 0x000000;
  leds[2] = 0x000000;leds[57] = 0x000000;leds[62] = 0x000000;leds[117] = 0x000000;leds[122] = 0x000000;leds[177] = 0x000000;leds[182] = 0x000000;leds[237] = 0x000000;leds[242] = 0x000000;leds[297] = 0x000000;leds[302] = 0x000000;leds[357] = 0x000000;leds[362] = 0x000000;leds[417] = 0x000000;leds[422] = 0x000000;leds[477] = 0x000000;leds[482] = 0x000000;leds[537] = 0x000000;leds[542] = 0x000000;leds[597] = 0x000000;
  leds[1] = 0x000000;leds[58] = 0x000000;leds[61] = 0x000000;leds[118] = 0x000000;leds[121] = 0x000000;leds[178] = 0x000000;leds[181] = 0x000000;leds[238] = 0x000000;leds[241] = 0x000000;leds[298] = 0x000000;leds[301] = 0x000000;leds[358] = 0x000000;leds[361] = 0x000000;leds[418] = 0x000000;leds[421] = 0x000000;leds[478] = 0x000000;leds[481] = 0x000000;leds[538] = 0x000000;leds[541] = 0x000000;leds[598] = 0x000000;
  leds[0] = 0x000000;leds[59] = 0x000000;leds[60] = 0x000000;leds[119] = 0x000000;leds[120] = 0x000000;leds[179] = 0x000000;leds[180] = 0x000000;leds[239] = 0x000000;leds[240] = 0x000000;leds[299] = 0x000000;leds[300] = 0x000000;leds[359] = 0x000000;leds[360] = 0x000000;leds[419] = 0x000000;leds[420] = 0x000000;leds[479] = 0x000000;leds[480] = 0x000000;leds[539] = 0x000000;leds[540] = 0x000000;leds[599] = 0x000000;
}

void loadMap(byte n){
  if(n){
    bool map[PLAYN][PLAYM] = { false };
    switch(n){
      case 1:
        map[0][19] = true;map[1][19] = true;map[2][19] = true;map[3][19] = true;map[6][19] = true;map[7][19] = true;map[8][19] = true;map[9][19] = true;map[10][19] = true;map[11][19] = true;map[12][19] = true;map[13][19] = true;map[16][19] = true;map[17][19] = true;map[18][19] = true;map[19][19] = true;
        map[0][18] = true;map[19][18] = true;
        map[0][17] = true;map[9][17] = true;map[19][17] = true;
        map[9][16] = true;
        map[9][15] = true;
        map[0][14] = true;map[9][14] = true;map[19][14] = true;
        map[0][13] = true;map[9][13] = true;map[19][13] = true;
        map[0][12] = true;map[9][12] = true;map[19][12] = true;
        map[0][11] = true;map[9][11] = true;map[19][11] = true;
        map[0][10] = true;map[9][10] = true;map[19][10] = true;
        map[0][9] = true;map[9][9] = true;map[10][9] = true;map[19][9] = true;
        map[0][8] = true;map[10][8] = true;map[19][8] = true;
        map[0][7] = true;map[10][7] = true;map[19][7] = true;
        map[0][6] = true;map[10][6] = true;map[19][6] = true;
        map[0][5] = true;map[10][5] = true;map[19][5] = true;
        map[10][4] = true;
        map[10][3] = true;
        map[0][2] = true;map[10][2] = true;map[19][2] = true;
        map[0][1] = true;map[19][1] = true;
        map[0][0] = true;map[1][0] = true;map[2][0] = true;map[3][0] = true;map[6][0] = true;map[7][0] = true;map[8][0] = true;map[9][0] = true;map[10][0] = true;map[11][0] = true;map[12][0] = true;map[13][0] = true;map[16][0] = true;map[17][0] = true;map[18][0] = true;map[19][0] = true;
        break;
      case 2:
        map[9][19] = true;
        map[9][18] = true;
        map[9][17] = true;map[10][17] = true;
        map[10][16] = true;
        map[10][15] = true;
        map[10][14] = true;
        map[10][13] = true;
        map[9][12] = true;map[10][12] = true;
        map[9][11] = true;
        map[9][10] = true;
        map[9][9] = true;
        map[9][8] = true;
        map[9][7] = true;map[10][7] = true;
        map[10][6] = true;
        map[10][5] = true;
        map[10][4] = true;
        map[10][3] = true;
        map[9][2] = true;map[10][2] = true;
        map[9][1] = true;
        map[9][0] = true;
        break;
      case 3:
        map[0][19] = true;map[1][19] = true;map[2][19] = true;map[3][19] = true;map[4][19] = true;map[5][19] = true;map[6][19] = true;map[7][19] = true;map[8][19] = true;map[9][19] = true;map[10][19] = true;map[11][19] = true;map[12][19] = true;map[13][19] = true;map[14][19] = true;map[15][19] = true;map[16][19] = true;map[17][19] = true;map[18][19] = true;map[19][19] = true;
        map[0][18] = true;map[4][18] = true;map[8][18] = true;map[12][18] = true;map[16][18] = true;
        map[0][17] = true;map[2][17] = true;map[4][17] = true;map[6][17] = true;map[8][17] = true;map[10][17] = true;map[12][17] = true;map[14][17] = true;map[16][17] = true;map[18][17] = true;
        map[0][16] = true;map[2][16] = true;map[4][16] = true;map[6][16] = true;map[8][16] = true;map[10][16] = true;map[12][16] = true;map[14][16] = true;map[16][16] = true;map[18][16] = true;
        map[0][15] = true;map[2][15] = true;map[4][15] = true;map[6][15] = true;map[8][15] = true;map[10][15] = true;map[12][15] = true;map[14][15] = true;map[16][15] = true;map[18][15] = true;
        map[0][14] = true;map[2][14] = true;map[4][14] = true;map[6][14] = true;map[8][14] = true;map[10][14] = true;map[12][14] = true;map[14][14] = true;map[16][14] = true;map[18][14] = true;
        map[0][13] = true;map[2][13] = true;map[4][13] = true;map[6][13] = true;map[8][13] = true;map[10][13] = true;map[12][13] = true;map[14][13] = true;map[16][13] = true;map[18][13] = true;
        map[0][12] = true;map[2][12] = true;map[4][12] = true;map[6][12] = true;map[8][12] = true;map[10][12] = true;map[12][12] = true;map[14][12] = true;map[16][12] = true;map[18][12] = true;
        map[0][11] = true;map[2][11] = true;map[4][11] = true;map[6][11] = true;map[8][11] = true;map[10][11] = true;map[12][11] = true;map[14][11] = true;map[16][11] = true;map[18][11] = true;
        map[0][10] = true;map[2][10] = true;map[4][10] = true;map[6][10] = true;map[8][10] = true;map[10][10] = true;map[12][10] = true;map[14][10] = true;map[16][10] = true;map[18][10] = true;
        map[0][9] = true;map[2][9] = true;map[4][9] = true;map[6][9] = true;map[8][9] = true;map[10][9] = true;map[12][9] = true;map[14][9] = true;map[16][9] = true;map[18][9] = true;
        map[0][8] = true;map[2][8] = true;map[4][8] = true;map[6][8] = true;map[8][8] = true;map[10][8] = true;map[12][8] = true;map[14][8] = true;map[16][8] = true;map[18][8] = true;
        map[0][7] = true;map[2][7] = true;map[4][7] = true;map[6][7] = true;map[8][7] = true;map[10][7] = true;map[12][7] = true;map[14][7] = true;map[16][7] = true;map[18][7] = true;
        map[0][6] = true;map[2][6] = true;map[4][6] = true;map[6][6] = true;map[8][6] = true;map[10][6] = true;map[12][6] = true;map[14][6] = true;map[16][6] = true;map[18][6] = true;
        map[0][5] = true;map[2][5] = true;map[4][5] = true;map[6][5] = true;map[8][5] = true;map[10][5] = true;map[12][5] = true;map[14][5] = true;map[16][5] = true;map[18][5] = true;
        map[0][4] = true;map[2][4] = true;map[4][4] = true;map[6][4] = true;map[8][4] = true;map[10][4] = true;map[12][4] = true;map[14][4] = true;map[16][4] = true;map[18][4] = true;
        map[0][3] = true;map[2][3] = true;map[4][3] = true;map[6][3] = true;map[8][3] = true;map[10][3] = true;map[12][3] = true;map[14][3] = true;map[16][3] = true;map[18][3] = true;
        map[0][2] = true;map[2][2] = true;map[4][2] = true;map[6][2] = true;map[8][2] = true;map[10][2] = true;map[12][2] = true;map[14][2] = true;map[16][2] = true;map[18][2] = true;
        map[2][1] = true;map[6][1] = true;map[10][1] = true;map[14][1] = true;map[18][1] = true;
        map[0][0] = true;map[1][0] = true;map[2][0] = true;map[3][0] = true;map[4][0] = true;map[5][0] = true;map[6][0] = true;map[7][0] = true;map[8][0] = true;map[9][0] = true;map[10][0] = true;map[11][0] = true;map[12][0] = true;map[13][0] = true;map[14][0] = true;map[15][0] = true;map[16][0] = true;map[17][0] = true;map[18][0] = true;map[19][0] = true;
        break;
      case 4:
        map[0][19] = true;map[1][19] = true;map[2][19] = true;map[3][19] = true;map[4][19] = true;map[5][19] = true;map[6][19] = true;map[7][19] = true;map[8][19] = true;map[9][19] = true;map[10][19] = true;map[11][19] = true;map[12][19] = true;map[13][19] = true;map[14][19] = true;map[15][19] = true;map[16][19] = true;map[17][19] = true;map[18][19] = true;map[19][19] = true;
        map[0][18] = true;map[4][18] = true;map[8][18] = true;map[12][18] = true;map[16][18] = true;
        map[0][17] = true;map[2][17] = true;map[4][17] = true;map[6][17] = true;map[8][17] = true;map[10][17] = true;map[12][17] = true;map[14][17] = true;map[16][17] = true;map[18][17] = true;
        map[0][16] = true;map[2][16] = true;map[4][16] = true;map[6][16] = true;map[8][16] = true;map[10][16] = true;map[12][16] = true;map[14][16] = true;map[16][16] = true;map[18][16] = true;
        map[0][15] = true;map[2][15] = true;map[4][15] = true;map[6][15] = true;map[8][15] = true;map[10][15] = true;map[12][15] = true;map[14][15] = true;map[16][15] = true;map[18][15] = true;
        map[0][14] = true;map[2][14] = true;map[4][14] = true;map[6][14] = true;map[8][14] = true;map[10][14] = true;map[12][14] = true;map[14][14] = true;map[16][14] = true;map[18][14] = true;
        map[0][13] = true;map[2][13] = true;map[4][13] = true;map[6][13] = true;map[8][13] = true;map[10][13] = true;map[12][13] = true;map[14][13] = true;map[16][13] = true;map[18][13] = true;
        map[0][12] = true;map[2][12] = true;map[4][12] = true;map[6][12] = true;map[8][12] = true;map[10][12] = true;map[12][12] = true;map[14][12] = true;map[16][12] = true;map[18][12] = true;
        map[0][11] = true;map[2][11] = true;map[4][11] = true;map[6][11] = true;map[8][11] = true;map[10][11] = true;map[12][11] = true;map[14][11] = true;map[16][11] = true;map[18][11] = true;
        map[0][10] = true;map[2][10] = true;map[4][10] = true;map[6][10] = true;map[8][10] = true;map[10][10] = true;map[12][10] = true;map[14][10] = true;map[16][10] = true;map[18][10] = true;
        map[0][9] = true;map[2][9] = true;map[4][9] = true;map[6][9] = true;map[8][9] = true;map[10][9] = true;map[12][9] = true;map[14][9] = true;map[16][9] = true;map[18][9] = true;
        map[0][8] = true;map[2][8] = true;map[4][8] = true;map[6][8] = true;map[8][8] = true;map[10][8] = true;map[12][8] = true;map[14][8] = true;map[16][8] = true;map[18][8] = true;
        map[0][7] = true;map[2][7] = true;map[4][7] = true;map[6][7] = true;map[8][7] = true;map[10][7] = true;map[12][7] = true;map[14][7] = true;map[16][7] = true;map[18][7] = true;
        map[0][6] = true;map[2][6] = true;map[4][6] = true;map[6][6] = true;map[8][6] = true;map[10][6] = true;map[12][6] = true;map[14][6] = true;map[16][6] = true;map[18][6] = true;
        map[0][5] = true;map[2][5] = true;map[4][5] = true;map[6][5] = true;map[8][5] = true;map[10][5] = true;map[12][5] = true;map[14][5] = true;map[16][5] = true;map[18][5] = true;
        map[0][4] = true;map[2][4] = true;map[4][4] = true;map[6][4] = true;map[8][4] = true;map[10][4] = true;map[12][4] = true;map[14][4] = true;map[16][4] = true;map[18][4] = true;
        map[0][3] = true;map[2][3] = true;map[6][3] = true;map[10][3] = true;map[14][3] = true;map[18][3] = true;
        map[0][2] = true;map[2][2] = true;map[3][2] = true;map[4][2] = true;map[5][2] = true;map[6][2] = true;map[7][2] = true;map[8][2] = true;map[9][2] = true;map[10][2] = true;map[11][2] = true;map[12][2] = true;map[13][2] = true;map[14][2] = true;map[15][2] = true;map[16][2] = true;map[17][2] = true;map[18][2] = true;
        map[0][1] = true;
        map[0][0] = true;map[1][0] = true;map[2][0] = true;map[3][0] = true;map[4][0] = true;map[5][0] = true;map[6][0] = true;map[7][0] = true;map[8][0] = true;map[9][0] = true;map[10][0] = true;map[11][0] = true;map[12][0] = true;map[13][0] = true;map[14][0] = true;map[15][0] = true;map[16][0] = true;map[17][0] = true;map[18][0] = true;map[19][0] = true;
        break;
    }
    for(byte j=0; j<PLAYM; j++){
      for(byte i=0; i<PLAYN; i++){
        if(map[i][j]) playArea[i][j] = MAPCOLOR;
      }
    }
  }
}

void gameOver(short size, __uint24 color){
  Serial.println(size);
  short counter;
  delay(200);
  for(byte i=0; i<2; i++){
    plotGameOver();
    FastLED.show();
    delay(1000);
    plotPlayArea();
    delay(1000);
  }
  plotGameOver();
  FastLED.show();
  delay(1000);
  clearPlayArea();
  counter = 0;
  for(byte j=0; j<PLAYM; j++){
    for(byte i=0; i<PLAYN; i++){
      if(counter<size){
        if(counter==size-1)    playArea[i][j] = HEADCOLOR; else
        if(counter<TAILSIZE-1) playArea[i][j] = tailColor[counter+1]; else
                               playArea[i][j] = color;
        playArea2px(i, j);
        FastLED.show();
        delay(10);
      }
      counter++;
    }
  }
  delay(5000);
  clearPlayArea();
  loadMap(MAP);
}

void initGame(){
  //splashScreen(); #TO DO
  plotBordas();
  plotCobra();
  loadMap(MAP);
}

class Food{
  private:
    byte x, y;
    __uint24 color;
  public:
    void init(){
      this->color = FOODCOLOR;
      newFood();
    }
    bool newFood(){
      short randNumber;
      short counter = 0;
      for(byte i=0; i<PLAYN; i++){
        for(byte j=0; j<PLAYM; j++){
          if(!playArea[i][j]){
            counter++;
          }
        }
      }
      randNumber = random(counter);
      counter = 0;
      for(byte i=0; i<PLAYN; i++){
        for(byte j=0; j<PLAYM; j++){
          if(playArea[i][j]==0){
            if(counter==randNumber){
              this->x = i;
              this->y = j;
              playArea[x][y] = color;
              playArea2px(x, y);
              return 0;
            }
            counter++;
          }
        }
      }
    }
};

class Snake{
  private:
    //short size; //Snake size. Used only for setup and scoring
    short head, tail; //Pointers for the head and tail index of body vector
    int8_t x, y; //Coordinates of the current head body part
    byte heading; //Direction of the head: 0=x+, 1=y+, 2=x-, 3=y- 
    __uint24 headColor; //Head color
    __uint24 color; //Head color
    short body[AREASIZE] = { 0 }; //Vector which has all body parts coordinates in [xxyy] format
  public:
    short size;
    void init(byte size, byte heading, byte x, byte y){
      //Serial.println("snake::init");
      this->size = size;
      this->heading = heading;
      this->x = x;
      this->y = y;
      this->color = SNAKECOLOR;
      this->headColor = HEADCOLOR;
      this->head = size-1;
      this->tail = 0;
      makeBody();
      Draw();
    }
    void makeBody(){
      switch(heading){
        case 0:
          for(short i=0; i<size; i++){
            body[i] = (x-size+i+1)*100+y;
          }
          break;
        case 1:
          for(short i=0; i<size; i++){
            body[i] = x*100+y-size+i+1;
          }
          break;
        case 2:
          for(short i=0; i<size; i++){
            body[i] = (x+size-i-1)*100+y;
          }
          break;
        case 3:
          for(short i=0; i<size; i++){
            body[i] = x*100+y+size-i-1;
          }
          break;
      }
    }
    void Draw() {
      for(short i=0; i<size; i++){
        if(i==size-1)    playArea[body2X(i)][body2Y(i)] = HEADCOLOR; else
        if(i<TAILSIZE-1) playArea[body2X(i)][body2Y(i)] = tailColor[i+1]; else
                         playArea[body2X(i)][body2Y(i)] = color;
      }
    }
    byte body2X(short index){
      return body[index]/100;
    }
    byte body2Y(short index){
      return body[index]-body[index]/100*100;
    }
    bool Next(Food &food){
      //byte tailX, tailY, 
      byte neckX, neckY;
      byte tailX[TAILSIZE];
      byte tailY[TAILSIZE];
      neckX = x;
      neckY = y;
      switch(heading){
        case 0:
          this->x++;
          if(x>=PLAYN) this->x = 0;
          break;
        case 1:
          this->y++;
          if(y>=PLAYM) this->y = 0;
          break;
        case 2:
          this->x--;
          if(x<0) this->x = PLAYN-1;
          break;
        case 3:
          this->y--;
          if(y<0) this->y = PLAYM-1;
          break;
      }
      goHead(); //Increment the Head and Neck pointer
      body[head] = x*100+y; //Add the Head coordinates in the body stack
      for(byte i=0; i<TAILSIZE; i++){
        tailX[i] = body2X((tail+i)%(AREASIZE)); //Get x coord of whole Tail
        tailY[i] = body2Y((tail+i)%(AREASIZE)); //Get y coord of whole Tail
      }
      switch(CheckColision()){
        case 0: //No colision
          playArea[tailX[0]][tailY[0]] = 0; //Turn off Tail LED
          goTail(); //Increment the Tail pointer
          break;
        case 1: //Catch food
          this->size++; //Increase size
          food.newFood(); //Generate new Food
          period = PERIOD0 - (size-4)/4*10; //Update speed
          if(period<MINPERIOD) period = MINPERIOD;
          break;
        case 2: //Collided
          Serial.println("COLIDIU");
          gameOver(size, color);
          init(SIZE0, HEADING0, X0, Y0);
          food.init();
          plotPlayArea();
          period = PERIOD0;
          return 0;
          break;
      }
      playArea[x][y] = headColor;
      playArea2px(x, y);
      playArea[neckX][neckY] = color;
      playArea2px(neckX, neckY);
      playArea[tailX[0]][tailY[0]] = 0;
      playArea2px(tailX[0], tailY[0]);
      for(byte i=1; i<TAILSIZE; i++){
        if(i<size){
          playArea[tailX[i]][tailY[i]] = tailColor[i]; //leds[i] = ColorFromPalette(bluePalette, i);
          playArea2px(tailX[i], tailY[i]);
        }
      }
      FastLED.show();
    }
    byte CheckColision(){
      if(playArea[x][y]==FOODCOLOR){
        return 1;
      }
      if(playArea[x][y]!=0){
        return 2;
      }
      return 0;
    }
    void goHead(){
      this->head++;
      if (head >= AREASIZE){
        this->head = 0;
      }
    }
    void goTail(){
      this->tail++;
      if (tail >= AREASIZE){
        this->tail = 0;
      }
    }
    void goRight(){
      this->heading--;
      this->heading = heading%4;
    }
    void goLeft(){
      this->heading++;
      this->heading = heading%4;
    }
};

Snake snake;
Food food;

void setup() {
  Serial.begin(9600);
  Serial.println("setup");
  pinMode(RIGHT_BTN, INPUT_PULLUP);
  pinMode(LEFT_BTN, INPUT_PULLUP);
  //randomSeed(42);
  randomSeed(analogRead(1));
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear();
  initGame();
  delay(200);
  for(byte i=0; i<TAILSIZE; i++){
    tailColor[i] = ((long)ColorFromPalette(bluePalette, i).r << 16) | ((long)ColorFromPalette(bluePalette, i).g << 8 ) | (long)ColorFromPalette(bluePalette, i).b; // get value and convert.
  }
  snake.init(SIZE0, HEADING0, X0, Y0);
  food.init();
  plotPlayArea();
  startMillis = millis();  //initial start time
  startTopMillis = millis();  //initial start time
}

void playAreaLoop(){
  currentMillis = millis();
  if(currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    snake.Next(food);
    startMillis = currentMillis;
  }
}

bool headSprite[6][6] = {
  {0, 1, 1, 1, 1, 0},
  {1, 1, 1, 1, 1, 1},
  {0, 0, 0, 1, 1, 1},
  {1, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 1, 1},
  {0, 1, 1, 1, 1, 0}
  };

short animateCobra(short step, short size){
  short x0 = step;
  short y0 = 1;
  short xS, yS;
  for(byte x=0; x<TOPN; x++){
    for(byte y=0; y<TOPM; y++){
      topArea[x][y] = 0;
      xS = -(x-x0);
      yS = y-y0;
      if(xS>=0 && xS<6 && yS>=0 && yS<6){
        if(headSprite[yS][xS]==1){
          topArea[x][y] = HEADCOLOR;
        }
      }
      if(xS>=6){
        if(xS<size*4+9){
          if(yS==0){
            if(xS<size*4+6 && step%2==0 && (xS+2*step+2)%8>4){
              topArea[x][y] = SNAKECOLOR;
              continue;
            }
          }
          if(yS==1){
            if((xS<size*4+8) && step%2){
              topArea[x][y] = SNAKECOLOR;
              continue;
            }
            if(xS<size*4+8 && (xS+2*step+1)%8>2){
              topArea[x][y] = SNAKECOLOR;
              continue;
            }
          }
          if(yS==2 || yS==3){
            topArea[x][y] = SNAKECOLOR;
            continue;
          }
          if(yS==4){
            if((xS<size*4+8) && step%2){
              topArea[x][y] = SNAKECOLOR;
              continue;
            }
            if(xS<size*4+8 && (xS+2*step+5)%8>2){
              topArea[x][y] = SNAKECOLOR;
              continue;
            }
          }
          if(yS==5){
            if(xS<size*4+6 && step%2==0 && (xS+2*step+6)%8>4){
              topArea[x][y] = SNAKECOLOR;
              continue;
            }
          }
        }
      }
    }
  }
  plotTopArea();
  return step+1;
}

void topScreenLoop(){
  static short step = 0;
  static short size;
  currentTopMillis = millis();
  if(currentTopMillis - startTopMillis >= topPeriod)  //test whether the period has elapsed
  {
    switch(topAnimation){
      case 0:
        plotCobra();
        topAnimation = 1;
        topPeriod = 5000;
        break;
      case 1:
        if(step==0) size = snake.size;
        step = animateCobra(step, size);
        topPeriod = 100;
        if(step > size*4+28){
          step = 0;
          topAnimation = 0;
        }
        break;
    }
    startTopMillis = currentTopMillis;
  }
}

void loop(){
  playAreaLoop();
  topScreenLoop();
  
  if(!rightPressed && digitalRead(RIGHT_BTN) == HIGH){
    //Serial.println("direita");
    snake.goRight();
    rightPressed = true;
    delay(10);
  }
  if(!leftPressed && digitalRead(LEFT_BTN) == HIGH){
    //Serial.println("esquerda");
    snake.goLeft();
    leftPressed = true;
    delay(10);
  }
  if(rightPressed && digitalRead(RIGHT_BTN) == LOW){
    //Serial.println("direita off");
    rightPressed = false;
    delay(10);
  }
  if(leftPressed && digitalRead(LEFT_BTN) == LOW){
    //Serial.println("esquerda off");
    leftPressed = false;
    delay(10);
  }
}

