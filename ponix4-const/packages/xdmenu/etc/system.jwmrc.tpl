<?xml version="1.0"?>

<JWM>

   <!-- The root menu, if this is undefined you will not get a menu. -->
   <!-- Additional RootMenu attributes: onroot, labeled, label -->

   <!-- Additional tray attributes: autohide, width, border, layer, layout -->
   <Tray layout="$WM_TRAY_LAYOUT" valign="$WM_TRAY_VALIGN" halign="$WM_TRAY_HALIGN" autohide="$WM_TRAY_AUTOHIDE">

      <!-- Additional TrayButton attribute: label -->
      <TrayButton icon="config.xpm" popup="$LNG_WM_START_CM">exec:/etc/xdmenu/scripts/connection_manager</TrayButton>

      <TaskList maxwidth="256"/>

      <Dock/>

   </Tray>

   <!-- Visual Styles -->

   <WindowStyle>

      <Font>Terminus</Font>
      <Width>4</Width>
      <Height>20</Height>

      <Active>
         <Text>white</Text>
         <Title>#70849d:#2e3a67</Title>
         <Outline>black</Outline>
         <Opacity>1.0</Opacity>
      </Active>

      <Inactive>
         <Text>#aaaaaa</Text>
         <Title>#808488:#303438</Title>
         <Outline>black</Outline>
         <Opacity>1.0:1.0:1.0</Opacity>
      </Inactive>

   </WindowStyle>

   <TaskListStyle>
      <Font>Terminus</Font>
      <ActiveForeground>black</ActiveForeground>
      <ActiveBackground>gray80:gray90</ActiveBackground>
      <Foreground>black</Foreground>
      <Background>gray90:gray80</Background>
   </TaskListStyle>

   <!-- Additional TrayStyle attribute: insert -->
   <TrayStyle>
      <Font>Terminus</Font>
      <Background>gray90</Background>
      <Foreground>black</Foreground>
      <Opacity>1.0</Opacity>
   </TrayStyle>

   <PagerStyle>
      <Outline>black</Outline>
      <Foreground>gray90</Foreground>
      <Background>#808488</Background>
      <ActiveForeground>#70849d</ActiveForeground>
      <ActiveBackground>#2e3a67</ActiveBackground>
   </PagerStyle>

   <MenuStyle>
      <Font>Terminus</Font>
      <Foreground>black</Foreground>
      <Background>gray90</Background>
      <ActiveForeground>white</ActiveForeground>
      <ActiveBackground>#70849d:#2e3a67</ActiveBackground>
      <Opacity>1.0</Opacity>
   </MenuStyle>

   <PopupStyle>
      <Font>Terminus</Font>
      <Outline>black</Outline>
      <Foreground>black</Foreground>
      <Background>yellow</Background>
   </PopupStyle>

   <IconPath>
      /share/icons/
   </IconPath>

   <Desktops width="1" height="1">  
      <Background type="image">ponix.xpm</Background>
   </Desktops>

   <!-- Double click speed (in milliseconds) -->
   <DoubleClickSpeed>400</DoubleClickSpeed>

   <!-- Double click delta (in pixels) -->
   <DoubleClickDelta>2</DoubleClickDelta>

   <!-- The focus model (sloppy or click) -->
   <FocusModel>click</FocusModel>

   <!-- The snap mode (none, screen, or border) -->
   <SnapMode distance="10">border</SnapMode>

   <!-- The move mode (outline or opaque) -->
   <MoveMode>outline</MoveMode>

   <!-- The resize mode (outline or opaque) -->
   <ResizeMode>outline</ResizeMode>

   <!-- Key bindings -->
   <Key key="Up">up</Key>
   <Key key="Down">down</Key>
   <Key key="Right">right</Key>
   <Key key="Left">left</Key>
   <Key key="h">left</Key>
   <Key key="j">down</Key>
   <Key key="k">up</Key>
   <Key key="l">right</Key>
   <Key key="Return">select</Key>
   <Key key="Escape">escape</Key>

   <Key mask="A" key="Tab">nextstacked</Key>
   <Key mask="A" key="F4">close</Key>
   <Key mask="A" key="#">desktop#</Key>
   <Key mask="A" key="F2">window</Key>
   <Key mask="A" key="F10">maximize</Key>
   <Key mask="A" key="Right">rdesktop</Key>
   <Key mask="A" key="Left">ldesktop</Key>
   <Key mask="A" key="Up">udesktop</Key>
   <Key mask="A" key="Down">ddesktop</Key>

</JWM>
