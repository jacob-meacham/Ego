if(25) {
Plant: You must be a mind reader, since I didn't tell you I needed water.;
Plant: But I did, and I will take it, and give you this.;
GetItem(Soil, 1);
}
else {
EGO: Here you are, my fine sir.;
Plant: Wow, I didn't think you'd actually find some.;
EGO: It was no proble-;
Plant: It only took you around 2 hours.;
Plant: I could have evolved legs and gotten it myself in that time.;
Plant: But I suppose I owe you something.;
Plant: Here, take some of my dirt.;
GetItem(Soil, 1);
EGO: Uh, thanks.;
}
Plant: By the way, see that floorboard in the middle of the room?  Its loose.;
CreateObject(LooseFloorboard);
LoseItem(WaterInventory);
SetVariable(Plant, 26, 0);
SetVariable(Plant, 1, 1);
stop