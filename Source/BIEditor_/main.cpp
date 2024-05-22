#include <QApplication>
#include "QBIEditor.h"
#include "BIEditor.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  std::shared_ptr<BIEditorLogic> pBIGameLogic = std::make_shared<BIEditorLogic>();
  BIEditorApp BIGameApp(pBIGameLogic);

  if (!BIEngine::g_pApp)
      return -1;

  //Это позволяет нам использовать ресурсы из обычной папки, а не Zip
  BIEngine::g_pApp->m_options.useDevelopmentAssets = true;

  QBIEditor win;
  win.show();

  //Это здесь из-за того, что OpenGL иниициализируется в QT перед первым показом
  //Затем мы инициализируем наши системы, половина из которых завязана на OpenGL
  win.Init();

  return a.exec();
}
