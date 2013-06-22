EGO: Hello, is anyone bac--;
Screen: I'm trying to change here, now don't bother me!;
EGO: I just want to talk to you.;
Screen: Go away!;
begin conversation
{
show 1
show 2
show 3
show 9
}
@1 Who are you?;
EGO: Who are you?;
Screen: Why, the owner of this dressing room, of course!;
EGO: And who is that?;
Screen: Jimmy Van Eice.;
if(1) {
EGO: Good enough for me.;
Wait(0);
}
else {
EGO: Ha! I know for a fact that the real owner of this is named Toperwaller SpinBottom!;
Screen: Preferably when you are gone.;
EGO: What?;
Screen: On the bottom.;
EGO: ...;
Screen: Please turn tape over to side two.;
SetVariable(Screen, 2, 0);
}
{
hide 1
show 1
}
end
@2 When will you be done changing?;
EGO: When will you be done changing?;
Screen: Preferably when you are gone.;
{
hide 2
show 2
}
end
@3 Answer me this riddle:;
EGO: Answer me this riddle:;
Wait(0);
{
hide 1
hide 2
hide 3
hide 9
show 4
show 5
show 6
show 7
show 8
}
end
@4 How do rabbits travel?;
EGO: How do rabbits travel?;
Screen: By hareplane.;
{
hide 4
}
end
@5 What kind of soda must you never drink?;
EGO: What kind of soda must you never drink?;
Screen: Baking soda.;
{
hide 5
}
end
@6 Where is the ocean the deepest?;
EGO: Where is the ocean the deepest?;
Screen: On the bottom.;
{
hide 6
}
end
@7 What do you need to spot an iceberg 25 miles away?;
EGO: What do you need to spot an iceberg 25 miles away?;
Screen: Good Eyesight.;
Plant: Boo!;
{
hide 7
}
end
@8 I've had enough of these riddles.;
EGO: I've had enough of these riddles.;
Wait(0);
{
show 1
show 2
show 3
show 9
hide 4
hide 5
hide 6
hide 7
hide 8
}
end
@9 I've got better things to do.;
EGO: I've got better things to do.;
Screen: Oh yeah? Name one.;
stop
@10 Im Unused;
EGO: Im unused;
Wait(0);
stop
end conversation
stop