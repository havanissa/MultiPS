
void VHF_radio_start()
{
  radio.powerUp();
  radio.setTXpower(115);  // dBuV, 88-115 max
  radio.tuneFM(FMSTATION);
  radio.beginRDS();
  radio.setRDSstation("Yarrrdio");
  radio.setRDSbuffer("Pirate radio!");
  radio.powerUp();
}
