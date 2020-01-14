#include <SPI.h>
#include <TrueRandom.h>
#include <SMARTWAV.h>
#include <elapsedMillis.h>
#include <Average.h>
elapsedMillis timeElapsed;
SMARTWAV sWav;
Average<float> runAveNG(10);
Average<float> lickAveR(3);
Average<float> lickAveL(3);
Average<float> lickBias(10);
Average<float> setBias(5);
Average<float> runAve(30); //***determines the moving average of block size for criterion
Average<float> runAveL(10);
Average<float> runAveR(10);

int session;
int sessionSEDS;
int sedsType = 1;
int interleave = 0;
String sedsPhase = "SEDSA";
String stimPhase = " ";
String sessionNames[] =  { "Compound Discrimination", "Intradimensional Shift", "Reversal", "Extradimensional shift / Intradimensional shift B" , "Serial Extradimensional Shifts"};
String sessionTag = " ";
int randLight;
float critScore = 0.8; //determines criterion score needed to change blocks
int randsetID = 0; //determines whether first set is randomized among the first four in the setChoose list
int seqNum = 0; //set to 0 to start from beginning of set/mapping sequence
char tone1[8] = {'C', 'L', 'K'}; //SD/CD: {'0', '3', '5'}, IDS1: {'M', 'O', 'D'}, IDS2: {'2', '1', '0'}
char tone2[8] = {'2', '1', '0'}; //SD/CD: {'1', '5', '5'}, IDS1: {'S', 'W', 'P'}, IDS2: {'C', 'L', 'K'}

int delayTimeL = 35; //35; //open time on left solenoid
int delayTimeR = 35; //35; //open time on right solenoid
int trialMax = 350;
int LBCorrect = 1; //correct lickBias -- 0 for no, 1 for yes
//int lightYN [] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //set to 1 for no light first block alternating, set to -1 for light first block alternating, set to 0 for no light any blocks
int trialLight;
int toneDur = 500;
int mapLR [] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; //assign reversals -- 1 for no reversal, -1 for reversal
int setChoose [] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; //assign set -- 1 for whisker, -1 for odor

int initRule;
int EDScomplete=0;
int toneLR;
int odorLR;
int keyDetect;
int blockNum = 0;
int blockMax = 1;
int volMax = 50;
int setID; 
int mapID=1;
int trialLR;
int stimLR;
int irrelLR;
char trialTone[8] = {'W', 'H', 'N'}; //
char toneCorrectL[8] = {'W', 'H', 'L'}; //correct left
char toneCorrectR[8] = {'W', 'H', 'R'}; //correct right
int trialChooseLB [] = { -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1};//{-1,1};//for running without lickBias
int trialChoose [] = { -1, 1};//{-1,1};//for running without lickBias
int irrelChoose [] = { -1, -1, 1, 1}; // choose irrelevant stim -- -1 for incongruent, 1 for congruent
int lightChoose [] = { 0, 1};
int blocInit [] = {0, 1, 2, 3};
int crit=0;
int ltCong=0;
int ITIhalf=0;

int LEDChannel = 53;
int lightChan = 13;
int H2OL = 7;
int H2OR = 6;
int odorAirSupply = 10;
int odorAB = 11; //LOW=A;HIGH=B
int setIDOut = 44;
int trialOut = 46;
int trialLROut = 47;
int irrelLROut = 48;
int lickingLOut = 49;
int lickingROut = 50;
int rewardOut = 51;
int toneClockOut = 52;

int brightness = 250;
int fadeAmount = 5;
int lickThresh = 10;
int lickPrev = 0;
int odorID = 1; //0=A,1=B
int toneID = 1;
int RT = 0;
int ITI = 0;
int trial = 0;
int lickedR = 0;
int lickedL = 0;
int lickNumR = 0;
int lickNumL = 0;
int trialCorrect = 0;
int totalDispensed = 0;
int waiting = 1;

void setup() {
  Serial.begin(9600);
  pinMode(LEDChannel, OUTPUT); digitalWrite(LEDChannel, HIGH);
  pinMode(lightChan, OUTPUT); digitalWrite(lightChan, LOW);
  pinMode(12, OUTPUT); digitalWrite(12, LOW);
  pinMode(3, OUTPUT);
  pinMode(9, OUTPUT);digitalWrite(9, LOW);
  Serial.begin(9600);
  pinMode(H2OL, OUTPUT);
  pinMode(H2OR, OUTPUT);
  pinMode(odorAirSupply, OUTPUT);digitalWrite(odorAirSupply, LOW);
  pinMode(odorAB, OUTPUT); digitalWrite(odorAB, LOW); //Low=A,High=B;
  digitalWrite(H2OL, LOW);
  digitalWrite(H2OR, LOW);
  pinMode(setIDOut, OUTPUT); digitalWrite(setIDOut, LOW);
  pinMode(trialOut, OUTPUT); digitalWrite(trialOut, LOW);
  pinMode(trialLROut, OUTPUT);digitalWrite(trialLROut, LOW);
  pinMode(irrelLROut, OUTPUT);digitalWrite(irrelLROut, LOW);
  pinMode(lickingLOut, OUTPUT);digitalWrite(lickingLOut, LOW);
  pinMode(lickingROut, OUTPUT);digitalWrite(lickingROut, LOW);
  pinMode(rewardOut, OUTPUT);digitalWrite(rewardOut, LOW);
      digitalWrite(H2OL,HIGH);
      delay(delayTimeL);
      digitalWrite(H2OL,LOW);
      digitalWrite(H2OR,HIGH);
      delay(delayTimeR);
      digitalWrite(H2OR,LOW);
//      digitalWrite(H2OL,HIGH);
//      delay(delayTimeL);
//      digitalWrite(H2OL,LOW);
//      digitalWrite(H2OR,HIGH);
//      delay(delayTimeR);
//      digitalWrite(H2OR,LOW);


  //Set task phase
  delay(500);
  Serial.println("Choose session:");
  Serial.println("(1) CD"); Serial.println("(2) IDS");Serial.println("(3) Rev"); Serial.println("(4) EDS/IDSB");Serial.println("(5) SEDS");//Serial.println("(6) SRev");
  Serial.println(" ");
  while (waiting == 1) {
    if (Serial.available() > 0) {    // is a character available?
      session = Serial.parseInt();       // get the character
      if (session > 0 && session <=6) {
        sessionTag = sessionNames[ session - 1 ];
        waiting = 0;
      }
    }
  }

if(session<5){
//Specify initial rule from training
waiting=1;
    Serial.println("Animal initially trained on (0) odor rule or (1) whisker rule?");
    delay(500);
    while (waiting == 1) {
      if (Serial.available() > 0) {    // is a character available?
        initRule = Serial.parseInt();       // get the character 
        if(initRule==0){
          seqNum=seqNum+1;
          Serial.println("Odor Rule");
          } 
          if(initRule==1){
            Serial.println("Whisker Rule");
          }
        if (initRule == 0 || initRule == 1) {
          waiting = 0;
        }
      }
    }
  }
if(session>1 && session<5 && initRule==0){Serial.println("Make sure odor valves are swtiched from odors A/B to odors C/D");}

if(initRule==0 && session==4){ //differentiate between EDS and IDSB for new whisker stims
//Set light stimulation
waiting=1;
    Serial.println("Has EDS been completed? (0 = No, 1 = Yes)");
    while (waiting == 1) {
      if (Serial.available() > 0) {    // is a character available?
        EDScomplete = Serial.parseInt();       // get the character
        if (EDScomplete == 0 || EDScomplete == 1) {
          waiting = 0;
        }
      }
    }
  }

//Set light stimulation
waiting=1;
    Serial.println("Light stimulation trials? (0 for no, 1 for inhibit, 2 for excite)");
    Serial.println("IF YES, MAKE SURE FIBER IS PLUGGED IN!!!");
    delay(1000);
    while (waiting == 1) {
      if (Serial.available() > 0) {    // is a character available?
        randLight = Serial.parseInt();       // get the character
        if(randLight>=1 && session<5){
          LBCorrect=0;
          } //remove lick bias correction if testing with light stim
        if (randLight == 0 || randLight == 1 || randLight == 2) {
          waiting = 0;
        }
      }
    }
//
//if (session == 5 && randLight>=1){
//    waiting=1;
//    Serial.println("Light stim during trial (1) or during ITI (2) ?");
//    while (waiting == 1) {
//      if (Serial.available() > 0) {    // is a character available?
//        keyDetect = Serial.parseInt();       // get the character
//        if (keyDetect == 2 || keyDetect == 1) {
//          if(keyDetect==2){
//            sedsType=1;
//            sedsPhase= "SEDSA";
//          } //ITI stimulation
//          Serial.println(sedsType);
//          waiting = 0;
//        }
//      }
//    }
//}

if (session ==5 && sedsType == 1 && randLight>=1){
    waiting=1;
    Serial.println("Light following congruent (1), incongruent (2), or all (3) trials?");
    while (waiting == 1) {
      if (Serial.available() > 0) {    // is a character available?
        keyDetect = Serial.parseInt();       // get the character
        if (keyDetect == 1 || keyDetect == 2 || keyDetect == 3) {
          if(keyDetect==1){
            ltCong=1;
            sedsPhase= "SEDSA_cong";
          } //ITI stimulation
          if(keyDetect==2){
            ltCong=0;
            sedsPhase= "SEDSA_incong";
          } //ITI stimulation
          if(keyDetect==3){
            ltCong=2;
            sedsPhase= "SEDSA_all";
          } //ITI stimulation
          waiting = 0;
        }
      }
    }
     waiting=1;
    Serial.println("Light during beginning (1), end (2), or entire (3) ITI?");
    while (waiting == 1) {
      if (Serial.available() > 0) {    // is a character available?
        keyDetect = Serial.parseInt();       // get the character
        if (keyDetect == 1 || keyDetect == 2 || keyDetect == 3) {
          if(keyDetect==1){
            ITIhalf=1;
            stimPhase= " light stim beginning of ITI";
          } //ITI stimulation
          if(keyDetect==2){
            ITIhalf=2;
            stimPhase= " light stim end of ITI";
          } //ITI stimulation
          if(keyDetect==3){
            ITIhalf=3;
            stimPhase= " light stim entire ITI";
          } //ITI stimulation
          Serial.print("Running ");
          Serial.print(sedsPhase);
          Serial.print(" with");
          Serial.println(stimPhase);
          waiting = 0;
}     }
}
}


//
//if (session == 5 && sedsType == 0 && randLight>=1){
//    waiting=1;
//    Serial.println("Light in alternating blocks (1), or randomly interleaved (2)");
//    while (waiting == 1) {
//      if (Serial.available() > 0) {    // is a character available?
//        keyDetect = Serial.parseInt();       // get the character
//        if (keyDetect == 1 || keyDetect == 2) {
//          if(keyDetect==1){
//            interleave=0;
//            sedsPhase= "Blocked_stim";
//          } 
//          if(keyDetect==2){
//            interleave=1;
//            sedsPhase= "Interleaved_stim";
//          }
//          waiting = 0;
//        }
//      }
//    }
//}

  if (session == 6 || session == 5){sessionSEDS = 0;} //?

//Set Whisker Stimuli 
  if ((initRule==1 && session >= 2) || (initRule==0 && ((session==4 && EDScomplete==1) || session>=5))) { //ID shift, new whisker stims
    tone1[0] = '0';tone1[1] = '3';tone1[2] = '5';
    tone2[0] = '1';tone2[1] = '5';tone2[2] = '5';
  }
  if (session >= 3) { //Reversal and all sessions after
    mapID=-1;
  }
  if (session == 4) { //ED shift 1
    seqNum = seqNum+1;
  }

  if (session >= 5) {
    blockMax = 20;
    
    if (sessionSEDS == 0) {  //SEDS sessions last for up to 1000 trials 
      randsetID = 1;
      trialMax = 1000;
    }
    else {  //nonSEDS sessions last for up to 350 trials
      seqNum = sessionSEDS;//? 
      trialMax = 350;
    }
  }
  else{
    critScore = 1.1;
    } //In SRev sessions, session does not terminate upon reaching criterion 

  
  if (session == 6) { //Select stim mapping sequence for serial reversal
    for (int q = 1; q < 16; q+=2){
    mapLR[q] = -1;
      }
  }
  else{
    for (int q = 1; q < 16; q+=2){
    setChoose[q] = -1; //make setChoose alternate 1/-1 unless doing pure serial reversal
      }
    }

  if (session < 5){
    Serial.print("Beginning ");  
    Serial.print(sessionTag);
    Serial.print(" session");
    if(randLight == 0 ){
      Serial.println(" without light stim");
    }
    else{
      Serial.println(" with light stim");
    }
  }
  
  //Serial.print("SEDS block# = ");Serial.println(sessionSEDS);
  Serial.print("tone1 = "); Serial.println(tone1);
  Serial.print("tone2 = "); Serial.println(tone2);

  sWav.init();  //configure the serial and pinout of arduino board for SMARTWAV support
  sWav.reset(); //perform an initial reset

  Serial.write(sWav.volume(volMax));
//  tone(toneClockOut, 100);
  for (int q = 0; q < 100; q++) {
    //runAveOLR.push(0.5); runAveORL.push(0.5); runAveWLR.push(0.5); runAveWRL.push(0.5);
    runAveNG.push(1); //?
    //    lickBias.push(0);
  }
  timeElapsed = 0;
  randomSeed(analogRead(0));
  trialLR = 1;

  Serial.println(" ");
  Serial.print("trial"); Serial.print(" ");
  Serial.print("blockNum"); Serial.print(" ");
  Serial.print("ITI"); Serial.print(" ");
  Serial.print("setID"); Serial.print(" ");
  Serial.print("trialLR"); Serial.print(" ");
  Serial.print("stimLR"); Serial.print(" ");
  Serial.print("irrelLR"); Serial.print(" ");
  Serial.print("lickNumL"); Serial.print(" ");
  Serial.print("lickNumR"); Serial.print(" ");
  Serial.print("RT"); Serial.print(" ");
  Serial.print("trialCorrect"); Serial.print(" ");
  Serial.print("runAveL"); Serial.print(" ");
  Serial.print("runAveR"); Serial.print(" ");
  Serial.print("runAve"); Serial.print(" ");
  Serial.print("runAveNG"); Serial.print(" ");
  Serial.print("lightYN"); Serial.print(" ");
  Serial.print("totalDispensed"); Serial.print(" ");
  if (randsetID == 1) {
    seqNum = blocInit[random(0, 4)];
  }
  delay(1000);
  
}













void loop()
{ 
  blockNum = blockNum + 1;
  if(blockNum > 1){LBCorrect=0;}
  seqNum = seqNum + 1;
  setID = setChoose[seqNum - 1];
    if(setID==-1){digitalWrite(setIDOut, LOW);} //?
    else{digitalWrite(setIDOut, HIGH);}
  if(session==6){mapID = mapLR[seqNum - 1];}
  for (int q = 0; q < 30; q++) {
    runAve.push(0);
    runAveL.push(0);
    runAveR.push(0);
  }
  while (runAve.mean() < critScore | runAveL.mean() < 0.5 | runAveR.mean() < 0.5) {
    if (trial <= trialMax && totalDispensed < 1100 && runAveNG.mean() >= 0.2 && blockNum <= blockMax) {
      if (LBCorrect == 1) {
        trialLR = trialChooseLB[round(random(3 + 2 * lickBias.mean(), 9 + 2 * lickBias.mean()))];
      }
      else {
        trialLR = trialChoose[random(0, 2)];
      }
      stimLR = trialLR * mapID;
      irrelLR = irrelChoose[random(0, 4)] * stimLR;
      if (mapLR[seqNum - 2] != mapLR[seqNum - 1]) {
        irrelLR = irrelLR * (-1); //ensures proper congruent/incongruent balance when reversing and shifting at same time
      }
      trial = trial + 1;
      ITI = timeElapsed;
      RT = 0;
      lickedR = 0; lickedL = 0;
      lickNumL = 0; lickNumR = 0;
      trialCorrect = 0;
//      if (lightYN[blockNum - 1] == 1) {} //for whole SEDS blocks with light stim

      //Choose stimuli
      if (setID == -1){
      odorLR = stimLR;
      toneLR = irrelLR;
      }
      else {
      odorLR = irrelLR;
      toneLR = stimLR;
      }
 
      //CHOOSE LIGHT STIMULUS
      if (randLight>=1){
        if (session==5 && interleave==0){ //SEDS
        if(blockNum!=1 && blockNum!=3 && blockNum!=5 && blockNum!=7 && blockNum!=9 && blockNum!=11 && blockNum!=13 && blockNum!=15)
          {trialLight=1;}
        else{trialLight=0;}
        }
        else{trialLight = lightChoose[random(0, 2)];}
      }

      if(trialLight==1 && randLight==1 && sedsType==0){digitalWrite(lightChan,HIGH);}
      else if(trialLight==1 && randLight==2 && sedsType==0){tone(lightChan,35);}
        else{digitalWrite(lightChan,LOW);noTone(lightChan);}

      //BEGIN TRIAL
      Serial.write(sWav.playTrackName(trialTone)); 
      digitalWrite(trialOut, HIGH); //?
      delay(toneDur);
        
      //BEGIN ODOR PRESENTATION
      if (odorLR == -1){ digitalWrite(odorAirSupply, HIGH);}
      else{ digitalWrite(odorAB, HIGH); //?
            digitalWrite(odorAirSupply, HIGH);}      
   
      Serial.write(sWav.stopTrack());
      delay(50);
      timeElapsed = 0;

        if(trialLR==-1){digitalWrite(trialLROut, LOW);}
        else{digitalWrite(trialLROut, HIGH);}
        if(irrelLR==-1){digitalWrite(irrelLROut, LOW);}
        else{digitalWrite(irrelLROut, HIGH);}
      
      //BEGIN WHISKER STIM PRESENTATION
      if (toneLR == 1) {Serial.write(sWav.playTrackName(tone2));}
      else{Serial.write(sWav.playTrackName(tone1));}      

      while (timeElapsed <= 2500) {
        lickedR = 0; lickedL = 0;
        lickAveR.push(analogRead(A5) - analogRead(A0));
        lickAveL.push(analogRead(A0) - analogRead(A5));
        if (lickAveR.mean() > lickThresh) {
          lickedR = 1;
          digitalWrite(lickingROut, HIGH);delay(20);digitalWrite(lickingROut, LOW);
        }
        else if (lickAveL.mean() > lickThresh) {
          lickedL = 1;
          digitalWrite(lickingLOut, HIGH);delay(20);digitalWrite(lickingLOut, LOW);
        }
        if (lickPrev == 0 && lickedR == 1) {
          lickNumR = lickNumR + 1;
        }
        else if (lickPrev == 0 && lickedL == 1) {
          lickNumL = lickNumL + 1;
        }
        if (lickedR == 1 || lickedL == 1) {
          lickPrev = 1;
        }
        else {
          lickPrev = 0;
        }
        delay(5);
      }
      Serial.write(sWav.stopTrack());
      digitalWrite(odorAB, LOW);
      digitalWrite(odorAirSupply, LOW);
        digitalWrite(trialLROut, LOW);
        digitalWrite(irrelLROut, LOW);
      lickedR = 0; lickedL = 0;
      delay(50);
      timeElapsed = 0;

      //WAIT FOR RESPONSE
      while (lickedR == 0 && lickedL == 0 && timeElapsed <= 1500) {
        if (analogRead(A5) - analogRead(A0) > lickThresh) {
          lickedR = 1;
          RT = timeElapsed;
          digitalWrite(lickingROut, HIGH);delay(20); digitalWrite(lickingROut, LOW);
        }
        else if (analogRead(A0) - analogRead(A5) > lickThresh) {
          lickedL = 1;
          RT = timeElapsed;
          digitalWrite(lickingLOut, HIGH);delay(20); digitalWrite(lickingLOut, LOW);
        }
      }

      //DELIVER LIGHT DURING ITI FOLLOWING INCONGRUENT TRIALS FOR SEDS TYPE 1
      if (sedsType==1){
if(ITIhalf==1 || ITIhalf==3){
if(ltCong==2){
        if (trialLight==1 && randLight==1){
        digitalWrite(lightChan,HIGH);}
        else if (trialLight==1 && randLight==2){
        tone(lightChan,35);}
        else {trialLight=0;}
}
if(ltCong==0){
        if (stimLR!=irrelLR && trialLight==1 && randLight==1){
        digitalWrite(lightChan,HIGH);}
        else if (stimLR!=irrelLR && trialLight==1 && randLight==2){
        tone(lightChan,35);}
        else {trialLight=0;}
}
if(ltCong==1){
        if (stimLR==irrelLR && trialLight==1 && randLight==1){
        digitalWrite(lightChan,HIGH);}
        else if (stimLR==irrelLR && trialLight==1 && randLight==2){
        tone(lightChan,35);}
        else {trialLight=0;}
}       
    }              
        }

        

      //RECORD RESULT AND DELIVER REWARD OR NOT1
      if (trialLR == 1) {
        if (lickedR == 1) {
          trialCorrect = 1;
          runAveR.push(1);
        }
        if (trialCorrect == 1) {
          digitalWrite(rewardOut, HIGH);digitalWrite(H2OR, HIGH);
          delay(delayTimeR);
          digitalWrite(H2OR, LOW);digitalWrite(rewardOut, LOW);
          delay(delayTimeL-delayTimeR);
          Serial.write(sWav.playTrackName(toneCorrectR));
          totalDispensed = totalDispensed + 3;
        }
        else if (runAveNG.mean() >= 0.3 && runAveNG.mean() <= 0.6) {
          digitalWrite(rewardOut, HIGH);digitalWrite(H2OR, HIGH);
          delay(delayTimeR);
          digitalWrite(H2OR, LOW);digitalWrite(rewardOut, LOW);
          delay(delayTimeL-delayTimeR);
          Serial.write(sWav.playTrackName(toneCorrectR));
          totalDispensed = totalDispensed + 3;
        }
        else if (lickedL == 1) {
          runAveR.push(0);
          Serial.write(sWav.playTrackName(toneCorrectR));
        }
      }
      else if (trialLR == -1) {
        if (lickedL == 1) {
          trialCorrect = 1;
          runAveL.push(1);
        }
        if (trialCorrect == 1) {
          digitalWrite(rewardOut, HIGH);digitalWrite(H2OL, HIGH);
          delay(delayTimeL);
          digitalWrite(H2OL, LOW);digitalWrite(rewardOut, LOW);
          Serial.write(sWav.playTrackName(toneCorrectL));
          totalDispensed = totalDispensed + 3;
        }
        else if (runAveNG.mean() >= 0.3 && runAveNG.mean() <= 0.6) {
          digitalWrite(rewardOut, HIGH);digitalWrite(H2OL, HIGH);
          delay(delayTimeL);
          digitalWrite(H2OL, LOW);digitalWrite(rewardOut, LOW);
          Serial.write(sWav.playTrackName(toneCorrectL));
          totalDispensed = totalDispensed + 3;
        }
        else if (lickedR == 1) {
          runAveL.push(0);
          Serial.write(sWav.playTrackName(toneCorrectL));
        }
      }
      
      if (lickedL == 1 || lickedR == 1) {
        runAveNG.push(1);
        runAve.push(trialCorrect);
      }
      else {
        runAveNG.push(0);
      }


      if (trialLR == -1) {
        if (trialCorrect == 0) {
          lickBias.push(-1);lickBias.push(-1);
        }
        else {
          lickBias.push(0);
        }
      }
      else {
        if (trialCorrect == 0) {
          lickBias.push(1);lickBias.push(1);
        }
        else {
          lickBias.push(0);
        }
      }

      if(crit == 0 && runAve.mean() >= 0.8 && runAveL.mean() >= 0.5 && runAveR.mean() >= 0.5 && runAveNG.mean() >= 0.8){crit=1;}

      digitalWrite(trialOut, LOW);

      timeElapsed = 0;
      delay(1000);
      if (sedsType==0){digitalWrite(lightChan,LOW);noTone(lightChan);}

      delay(3500);  //first half of ITI
      
      if(ITIhalf==1){digitalWrite(lightChan,LOW);noTone(lightChan);}

      else if(ITIhalf==2){
      if(ltCong==2){
        if (trialLight==1 && randLight==1){
        digitalWrite(lightChan,HIGH);}
        else if (trialLight==1 && randLight==2){
        tone(lightChan,35);}
        else{trialLight=0;}
        }
      else if(ltCong==0){
        if (stimLR!=irrelLR && trialLight==1 && randLight==1){
        digitalWrite(lightChan,HIGH);}
        else if (stimLR!=irrelLR && trialLight==1 && randLight==2){
        tone(lightChan,35);}
        else{trialLight=0;}
        }
      else if(ltCong==1){
        if (stimLR==irrelLR && trialLight==1 && randLight==1){
        digitalWrite(lightChan,HIGH);}
        else if (stimLR==irrelLR && trialLight==1 && randLight==2){
        tone(lightChan,35);}
        else {trialLight=0;}
        }
      }
      
      delay(random(3500, 5500)); // 8000, 10000

      //PRINT TRIAL DATA AT THE END OF THE ITI  
      Serial.println(" ");
      Serial.print(trial); Serial.print(" ");
      Serial.print(blockNum); Serial.print(" ");
      Serial.print(ITI); Serial.print(" ");
      Serial.print(setID); Serial.print(" ");
      Serial.print(trialLR); Serial.print(" ");
      Serial.print(stimLR); Serial.print(" ");
      Serial.print(irrelLR); Serial.print(" ");
      Serial.print(lickNumL); Serial.print(" ");
      Serial.print(lickNumR); Serial.print(" ");
      Serial.print(RT); Serial.print(" ");
      Serial.print(trialCorrect); Serial.print(" ");
      Serial.print(runAveL.mean()); Serial.print(" ");
      Serial.print(runAveR.mean()); Serial.print(" ");
      Serial.print(runAve.mean()); Serial.print(" ");
      Serial.print(runAveNG.mean()); Serial.print(" ");
      Serial.print(trialLight); Serial.print(" ");
      Serial.print(totalDispensed); Serial.print(" ");
    
    
    }


    
    else {
      for (int i = 0; i <= 10; i++) {
        Serial.println();
      }
      Serial.print("Session finished.");
      if (session < 9) {
        Serial.print("  Animal has ");
        if (crit == 0) {
          Serial.print("NOT ");
        }
        Serial.print("PASSED! Please log score in spreadsheet.");
        Serial.println(" Give ");
        Serial.print(1100 - totalDispensed);
        Serial.print(" uL H2O to supplement.");
      }
      else {
        Serial.print("Animal has performed ");
        Serial.print(blockNum);
        Serial.print(" trial blocks. Please log score in spreadsheet.");
        Serial.println(" Give ");
        Serial.print(1100 - totalDispensed);
        Serial.print(" uL H2O to supplement."); 
      }
      
      while (1) {digitalWrite(53, HIGH);
      digitalWrite(lightChan, LOW);
      }
      digitalWrite(H2OL,LOW);digitalWrite(H2OR,LOW);
      digitalWrite(odorAirSupply,LOW);digitalWrite(odorAB,LOW);
//      while (1) {
//        analogWrite(53, brightness);
//        brightness = brightness + fadeAmount;
//        if (brightness == 0 || brightness == 255) {
//          fadeAmount = -fadeAmount;
//        }
//        delay(10);
//      }
    }
  }
}
