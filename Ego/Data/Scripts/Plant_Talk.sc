if(0) {
EGO: I've heard that talking to plants makes them much healthier.;
EGO: Hello plant, how are you?;
Wait(0.7);
EGO: I say, plant, how are you?;
Plant: You know, its very rude not to ask my name first.;
EGO: You can talk! Uh, what IS your name?;
Plant: My spore named me Remus Leonard Johnston Griffin Black Smith III.;
SetVariable(Plant, 0, 0);
}
if(1) {
Plant: Have you found me that water yet?;
Wait(0);
}
if(26) {
Wait(0);
Wait(0);
}
begin conversation
{
show 1
show 2
show 5
}
@1 So, who owns this dressing room?;
EGO: So, who owns this dressing room?;
Plant: I can speak, you know, but I'm no potato.;
Wait(1.5);
Plant: I don't have eyes.;
EGO: Ah!;
Plant: Not the sharpest knife in the drawer, are you?;
{
show 1
}
end
@2 Do you have anything that can help me?;
EGO: Do you have anything that can help me?;
Plant: Well, I could maybe part with some of my soil,;
Plant: that is, if you do me a favor.;
{
hide 1
hide 2
hide 5
show 3
show 4
}
end
@3 Er, I wouldn't know where to put my mouth.;
EGO: Er, I wouldn't know where to put my mouth.;
Plant: Thoughtful, but I'd rather you'd find me some water.;
EGO: Ok, I'll do it.;
SetVariable(Plant, 1, 1);
SetVariable(Plant, 25, 0);
stop
@4 I don't think I want to do you any favors.;
EGO: I don't think I want to do you any favors.;
Plant: Suit yourself.;
{
show 1
show 2
hide 3
hide 4
show 5
}
end
@5 Its been great talking to you, I hope I've made you healthier.;
EGO: Its been great talking to you, I hope I've made you healthier.;
Plant: Healthier? Maybe.  Stupider? Certainly.;
stop
@6 I'm unused.;
EGO: I'm unused.;
Wait(0);
stop
end conversation
stop
