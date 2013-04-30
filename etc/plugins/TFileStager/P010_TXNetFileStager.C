void P010_TXNetFileStager()
{
   gPluginMgr->AddHandler("TFileStager", "^root:", "TNetXNGFileStager",
      "NetXNG", "TNetXNGFileStager(const char *)");
}
