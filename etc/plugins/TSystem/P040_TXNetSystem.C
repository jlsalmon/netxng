void P040_TXNetSystem()
{
   gPluginMgr->AddHandler("TSystem", "^root:", "TNetXNGSystem",
      "NetXNG", "TNetXNGSystem(const char *,Bool_t)");
}
