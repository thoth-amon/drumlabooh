/*
written at 2023-24 by Peter Semiletov
this code is the public domain
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "kits.h"
#include "utl.h"
#include "dsp.h"

#define XFILLER 4
#define YFILLER 16



CDocumentWindow::CDocumentWindow (const String &name,
                                  Colour backgroundColour,
                                  int requiredButtons,
                                  bool addToDesktop):
                                  juce::DocumentWindow (name, backgroundColour, requiredButtons, addToDesktop)
{
}


void CDocumentWindow::closeButtonPressed()
{
  setVisible (false);
  removeFromDesktop();
}


bool CDocumentWindow::keyPressed (const KeyPress &key)
{
  if (key == juce::KeyPress::escapeKey)
     {
      setVisible (false);
      removeFromDesktop();
     }

  return false;
}


CFx::CFx()
{
  int xoffs = XFILLER * 2;
  int yoffs = YFILLER * 2;

  addChildComponent (gr_group_analog);
  gr_group_analog.setText ("ANALOG");

  bt_analog.setButtonText ("ON");
  addAndMakeVisible (bt_analog);
  bt_analog.setTopLeftPosition (xoffs, yoffs);
  bt_analog.setSize (48, 32);

  addAndMakeVisible (sl_analog_amount);

  sl_analog_amount.setTopLeftPosition (bt_analog.getX(), bt_analog.getBottom());
  sl_analog_amount.setSize (82, 82);

  sl_analog_amount.setSliderStyle (juce::Slider::Rotary);
  sl_analog_amount.setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
  sl_analog_amount.setTooltip ("Amount");

  gr_group_analog.setTopLeftPosition (XFILLER, YFILLER);
  gr_group_analog.setSize (sl_analog_amount.getWidth() + XFILLER * 2,
                           sl_analog_amount.getHeight() + YFILLER + bt_analog.getHeight() + YFILLER);

  gr_group_analog.setVisible (true);

  xoffs = gr_group_analog.getWidth() + XFILLER * 2;
  yoffs = YFILLER * 2;

  //LP ///////////////////////////////

  addChildComponent (gr_group_lp);
  gr_group_lp.setText ("LP");

  bt_lp.setButtonText ("ON");
  addAndMakeVisible (bt_lp);
  bt_lp.setTopLeftPosition (xoffs, yoffs);
  bt_lp.setSize (48, 32);

  xoffs += bt_lp.getWidth();
  xoffs += XFILLER;

  addAndMakeVisible (sl_lp_cutoff);

  sl_lp_cutoff.setTopLeftPosition (xoffs, yoffs);
  sl_lp_cutoff.setSize (68, 32);
  sl_lp_cutoff.setSliderStyle (juce::Slider::LinearHorizontal);
  sl_lp_cutoff.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
  sl_lp_cutoff.setTooltip ("Cutoff");

  xoffs += sl_lp_cutoff.getWidth();
  xoffs += XFILLER;

  addAndMakeVisible (sl_lp_reso);
  
  sl_lp_reso.setTopLeftPosition (xoffs, yoffs);
  sl_lp_reso.setSize (68, 32);
  sl_lp_reso.setSliderStyle (juce::Slider::LinearHorizontal);
  sl_lp_reso.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
  sl_lp_reso.setTooltip ("Reso");

  xoffs += sl_lp_reso.getWidth();
  xoffs += XFILLER;

  gr_group_lp.setTopLeftPosition (gr_group_analog.getWidth() + XFILLER * 2, YFILLER);
  gr_group_lp.setSize (bt_lp.getWidth() + XFILLER + sl_lp_reso.getWidth() + XFILLER + sl_lp_reso.getWidth() + XFILLER ,
                       bt_lp.getHeight() + YFILLER * 2);

  gr_group_lp.setVisible (true);


  //HP ///////////////////////////////

  xoffs = gr_group_analog.getWidth() + XFILLER * 2;
  yoffs = gr_group_lp.getBottom() + YFILLER * 2;

  addChildComponent (gr_group_hp);
  gr_group_hp.setText ("HP");

  bt_hp.setButtonText ("ON");
  addAndMakeVisible (bt_hp);
  bt_hp.setTopLeftPosition (xoffs, yoffs);
  bt_hp.setSize (48, 32);

  xoffs += bt_hp.getWidth();
  xoffs += XFILLER;

  addAndMakeVisible (sl_hp_cutoff);

  sl_hp_cutoff.setTopLeftPosition (xoffs, yoffs);
  sl_hp_cutoff.setSize (68, 32);
  sl_hp_cutoff.setSliderStyle (juce::Slider::LinearHorizontal);
  sl_hp_cutoff.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
  sl_hp_cutoff.setTooltip ("Cutoff");

  xoffs += sl_hp_cutoff.getWidth();
  xoffs += XFILLER;

  addAndMakeVisible (sl_hp_reso);
  
  sl_hp_reso.setTopLeftPosition (xoffs, yoffs);
  sl_hp_reso.setSize (68, 32);
  sl_hp_reso.setSliderStyle (juce::Slider::LinearHorizontal);
  sl_hp_reso.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
  sl_hp_reso.setTooltip ("Reso");

  xoffs += sl_hp_reso.getWidth();
  xoffs += XFILLER;

  gr_group_hp.setTopLeftPosition (gr_group_lp.getX(), gr_group_lp.getBottom() + YFILLER);
  gr_group_hp.setSize (bt_hp.getWidth() + XFILLER + sl_hp_reso.getWidth() + XFILLER + sl_hp_reso.getWidth() + XFILLER,
                       bt_hp.getHeight() + YFILLER * 2);

  gr_group_hp.setVisible (true);

  setSize (gr_group_analog.getWidth() + XFILLER + gr_group_lp.getWidth() + XFILLER * 2,
           gr_group_lp.getHeight() + gr_group_lp.getHeight() + YFILLER * 3);

  //setAlwaysOnTop(true);
}


CFx::~CFx()
{
 //this ugly hack prevent crash with Slider attachments and their listener

  att_lp = nullptr;
  att_lp_cutoff = nullptr;
  att_lp_reso = nullptr;

  att_hp = nullptr;
  att_hp_cutoff = nullptr;
  att_hp_reso = nullptr;

  att_analog = nullptr;
  att_analog_amount = nullptr;
}


void CLed::paint(Graphics& g)
{
  if (is_on)
     g.fillAll(cl_on.withLightness(velocity));
  else
      g.fillAll(cl_off);
}


#ifndef MULTICHANNEL

CDrumCell::CDrumCell()
{
  addChildComponent (fx);
  addChildComponent (gr_group);

  int xoffs = XFILLER * 2;

  addAndMakeVisible (led);
  led.setTopLeftPosition (xoffs, YFILLER);
  led.setSize (16, 32);

  xoffs += led.getWidth();
  xoffs += XFILLER;


  addAndMakeVisible (cell_label);

  cell_label.setTopLeftPosition (xoffs, YFILLER);
  cell_label.setSize (160, 32);
  cell_label.setColour (juce::Label::textColourId, juce::Colours::black);
  cell_label.setColour (juce::Label::backgroundColourId, juce::Colour (180, 209, 220));
//  label.setColour (juce::Label::backgroundColourId, juce::Colour (255, 222, 89));
  cell_label.setFont (f_samplename_font);
  cell_label.setText ("EMPTY CELL", juce::dontSendNotification);

  xoffs += cell_label.getWidth();
  xoffs += XFILLER;

  wnd_fx = 0;

  addAndMakeVisible (bt_fx);

  bt_fx.setButtonText ("FX");
  bt_fx.setColour (TextButton::ColourIds::buttonColourId,  Colours::darkslategrey);
  bt_fx.setTopLeftPosition (xoffs, YFILLER);
  bt_fx.setSize (42, 32);

  xoffs += bt_fx.getWidth();
  xoffs += XFILLER;

  addAndMakeVisible (sl_pan);
  sl_pan.setTopLeftPosition (xoffs, YFILLER);
  sl_pan.setSize (68, 32);
  sl_pan.setRange (0.0f, 1.0f, 0.01f);
  sl_pan.setSliderStyle (juce::Slider::LinearHorizontal);
  sl_pan.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
  sl_pan.setTooltip ("Pan");

  xoffs += sl_pan.getWidth();
  xoffs += XFILLER;

  addAndMakeVisible (sl_vol);

  sl_vol.setTopLeftPosition (xoffs, YFILLER);
  sl_vol.setSize (104, 32);
  sl_vol.setSliderStyle (juce::Slider::LinearHorizontal);
  sl_vol.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
  sl_vol.setRange (-96, 12, 1);
  sl_vol.setSkewFactor (4);
  sl_vol.setTooltip ("Volume");

    //sl_gain.addListener (this);

  xoffs += sl_vol.getWidth();
  xoffs += XFILLER;

  bt_mute.setButtonText ("M");
  addAndMakeVisible (bt_mute);
  bt_mute.setTopLeftPosition (xoffs, YFILLER);
  bt_mute.setSize (48, 32);

  xoffs += bt_mute.getWidth();
  xoffs += XFILLER;

  gr_group.setVisible (true);
  gr_group.setSize (xoffs, 32 + YFILLER + YFILLER);

  setSize (xoffs + XFILLER, 32 + YFILLER + YFILLER);

  bt_fx.onClick = [this] {
                          if (! wnd_fx)
                             {
                              wnd_fx = new CDocumentWindow ("FX", Colours::darkslategrey, DocumentWindow::TitleBarButtons::closeButton, false);
                             // wnd_fx = new CDocumentWindow ("FX", juce::Colour (246, 226, 206), DocumentWindow::TitleBarButtons::closeButton, false);
                              wnd_fx->setContentNonOwned (&fx, true);
                             }

                          if (! wnd_fx)
                              return;

                          if (wnd_fx->isOnDesktop())
                              wnd_fx->removeFromDesktop();
                          else
                              wnd_fx->addToDesktop();//ComponentPeer::StyleFlags::windowIsTemporary | ComponentPeer::StyleFlags::windowHasTitleBar | ComponentPeer::StyleFlags::windowHasCloseButton);

                          wnd_fx->setVisible (! wnd_fx->isVisible());
                          wnd_fx->setTopLeftPosition (bt_fx.getScreenX() + bt_fx.getWidth(), bt_fx.getScreenY() + bt_fx.getHeight());
                         };
}

#else
//MULTICHANNEL VERSION

CDrumCell::CDrumCell()
{
  addChildComponent (fx);

  addChildComponent (gr_group);

  int xoffs = XFILLER * 2;

  addAndMakeVisible (led);
  led.setTopLeftPosition (xoffs, YFILLER);
  led.setSize (16, 32);

  xoffs += led.getWidth();
  xoffs += XFILLER;

  addAndMakeVisible (cell_label);

  cell_label.setTopLeftPosition (xoffs, YFILLER);
  cell_label.setSize (160, 32);
  cell_label.setColour (juce::Label::textColourId, juce::Colours::black);
  cell_label.setColour (juce::Label::backgroundColourId, juce::Colour (180, 209, 220));
//  label.setColour (juce::Label::backgroundColourId, juce::Colour (255, 222, 89));
  cell_label.setFont (f_samplename_font);
  cell_label.setText ("EMPTY CELL", juce::dontSendNotification);

  xoffs += cell_label.getWidth();
  xoffs += XFILLER;

  wnd_fx = 0;

  addAndMakeVisible (bt_fx);

  bt_fx.setButtonText ("FX");
  bt_fx.setColour (TextButton::ColourIds::buttonColourId,  Colours::darkslategrey);
  bt_fx.setTopLeftPosition (xoffs, YFILLER);
  bt_fx.setSize (42, 32);

  xoffs += bt_fx.getWidth();
  xoffs += XFILLER;

  gr_group.setVisible (true);
  gr_group.setSize (xoffs, 32 + YFILLER + YFILLER);

  setSize (xoffs + XFILLER, 32 + YFILLER + YFILLER);


  bt_fx.onClick = [this] {
                          if (! wnd_fx)
                             {
                              wnd_fx = new CDocumentWindow ("FX", Colours::darkslategrey, DocumentWindow::TitleBarButtons::closeButton, false);
                             // wnd_fx = new CDocumentWindow ("FX", juce::Colour (246, 226, 206), DocumentWindow::TitleBarButtons::closeButton, false);
                              wnd_fx->setContentNonOwned (&fx, true);
                             }

                          if (! wnd_fx)
                              return;

                          if (wnd_fx->isOnDesktop())
                              wnd_fx->removeFromDesktop();
                          else
                              wnd_fx->addToDesktop();//ComponentPeer::StyleFlags::windowIsTemporary | ComponentPeer::StyleFlags::windowHasTitleBar | ComponentPeer::StyleFlags::windowHasCloseButton);

                           wnd_fx->setVisible (! wnd_fx->isVisible());
                           wnd_fx->setTopLeftPosition (bt_fx.getScreenX() + bt_fx.getWidth(), bt_fx.getScreenY() + bt_fx.getHeight());
                         };
}

#endif


CDrumCell::~CDrumCell()
{
  if (wnd_fx)
    delete wnd_fx;
}


#ifndef MULTICHANNEL

void CDrumCell::attach_params (CAudioProcessorEditor *ed, int cellno)
{
  cell_number = cellno;

  std::string param_name = "vol" + std::to_string (cell_number);
  att_vol.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (ed->valueTreeState, param_name, sl_vol));

  param_name = "pan" + std::to_string (cell_number);
  att_pan.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (ed->valueTreeState, param_name, sl_pan));

  param_name = "mute" + std::to_string (cell_number);
  att_mute.reset (new juce::AudioProcessorValueTreeState::ButtonAttachment (ed->valueTreeState, param_name, bt_mute));

  param_name = "lp" + std::to_string (cell_number);
  fx.att_lp.reset (new juce::AudioProcessorValueTreeState::ButtonAttachment (ed->valueTreeState, param_name, fx.bt_lp));

  param_name = "lp_cutoff" + std::to_string (cell_number);
  fx.att_lp_cutoff.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (ed->valueTreeState, param_name, fx.sl_lp_cutoff));

  param_name = "lp_reso" + std::to_string (cell_number);
  fx.att_lp_reso.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (ed->valueTreeState, param_name, fx.sl_lp_reso));

  param_name = "hp" + std::to_string (cell_number);
  fx.att_hp.reset (new juce::AudioProcessorValueTreeState::ButtonAttachment (ed->valueTreeState, param_name, fx.bt_hp));

  param_name = "hp_cutoff" + std::to_string (cell_number);
  fx.att_hp_cutoff.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (ed->valueTreeState, param_name, fx.sl_hp_cutoff));

  param_name = "hp_reso" + std::to_string (cell_number);
  fx.att_hp_reso.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (ed->valueTreeState, param_name, fx.sl_hp_reso));

  param_name = "analog" + std::to_string (cell_number);
  fx.att_analog.reset (new juce::AudioProcessorValueTreeState::ButtonAttachment (ed->valueTreeState, param_name, fx.bt_analog));

  param_name = "analog_amount" + std::to_string (cell_number);
  fx.att_analog_amount.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (ed->valueTreeState, param_name, fx.sl_analog_amount));
}

#else
//MULTICHANNEL VERSION

void CDrumCell::attach_params (CAudioProcessorEditor *ed, int cellno)
{
  cell_number = cellno;

  std::string param_name = "lp" + std::to_string (cell_number);
  fx.att_lp.reset (new juce::AudioProcessorValueTreeState::ButtonAttachment (ed->valueTreeState, param_name, fx.bt_lp));

  param_name = "lp_cutoff" + std::to_string (cell_number);
  fx.att_lp_cutoff.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (ed->valueTreeState, param_name, fx.sl_lp_cutoff));

  param_name = "lp_reso" + std::to_string (cell_number);
  fx.att_lp_reso.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (ed->valueTreeState, param_name, fx.sl_lp_reso));

  param_name = "hp" + std::to_string (cell_number);
  fx.att_hp.reset (new juce::AudioProcessorValueTreeState::ButtonAttachment (ed->valueTreeState, param_name, fx.bt_hp));

  param_name = "hp_cutoff" + std::to_string (cell_number);
  fx.att_hp_cutoff.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (ed->valueTreeState, param_name, fx.sl_hp_cutoff));

  param_name = "hp_reso" + std::to_string (cell_number);
  fx.att_hp_reso.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (ed->valueTreeState, param_name, fx.sl_hp_reso));

  param_name = "analog" + std::to_string (cell_number);
  fx.att_analog.reset (new juce::AudioProcessorValueTreeState::ButtonAttachment (ed->valueTreeState, param_name, fx.bt_analog));

  param_name = "analog_amount" + std::to_string (cell_number);
  fx.att_analog_amount.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (ed->valueTreeState, param_name, fx.sl_analog_amount));
}
#endif


void CDrumCell::set_name (const std::string &n)
{
  juce::String s (juce::CharPointer_UTF8((n.c_str())));
  cell_label.setText (s, juce::dontSendNotification);
}


//load_kit just updates GUI, actual kit load is at CAudioProcessor::load_kit 
void CAudioProcessorEditor::load_kit()
{
  //std::string real_kitpath = transform_kit_path_to_local (kitpath);

  //make all drum labels empty
  
  for (size_t i = 0; i < 36; i++)
      {
       drumcells[i].set_name ("EMPTY CELL");
       drumcells[i].cell_label.setColour (juce::Label::backgroundColourId, juce::Colour (131, 152, 160));
      }
  
  if (! audioProcessor.drumkit)
     return;
 

  for (size_t i = 0; i < audioProcessor.drumkit->v_samples.size(); i++)
      {
       drumcells[i].set_name (audioProcessor.drumkit->v_samples[i]->name);
       drumcells[i].cell_label.setColour (juce::Label::backgroundColourId, juce::Colour (180, 209, 220));
      }

   juce::String kitname = audioProcessor.drumkit->kit_name.c_str();

   l_kitinfo.setText (kitname, juce::dontSendNotification );

   if (file_exists (audioProcessor.drumkit->image_fname))
      {
       juce::File fl (audioProcessor.drumkit->image_fname);
       juce::Image im = juce::ImageFileFormat::loadFrom (fl);
       kit_image.setImage(im);
      }
   else
       kit_image.setImage(juce::Image ());

}


/*
//load_kit just updates GUI, actual kit load is at CAudioProcessor::load_kit 
void CAudioProcessorEditor::load_kit (const std::string &kitpath)
{
  //make all drum labels empty

  std::string real_kitpath = transform_kit_path_to_local (kitpath);
  
  for (size_t i = 0; i < 36; i++)
      {
       drumcells[i].set_name ("EMPTY CELL");
       drumcells[i].cell_label.setColour (juce::Label::backgroundColourId, juce::Colour (131, 152, 160));
      }

  if (audioProcessor.scanner.v_scanned_kits.size() == 0)
      return;

  //find kit at v_scanned_kits

  CDrumKit *k = 0;

  for (size_t i = 0; i < audioProcessor.scanner.v_scanned_kits.size() ; i++)
      {
       if (audioProcessor.scanner.v_scanned_kits[i]->kit_filename == real_kitpath)
          k = audioProcessor.scanner.v_scanned_kits[i];
      }

  if (! k)
      return;

  for (size_t i = 0; i < k->v_samples.size(); i++)
      {
       drumcells[i].set_name (k->v_samples[i]->name);
       drumcells[i].cell_label.setColour (juce::Label::backgroundColourId, juce::Colour (180, 209, 220));
      }

   juce::String kitname = k->kit_name.c_str();

   l_kitinfo.setText (kitname, juce::dontSendNotification );

   if (file_exists (k->image_fname))
      {
       juce::File fl (k->image_fname);
       juce::Image im = juce::ImageFileFormat::loadFrom (fl);
       kit_image.setImage(im);
      }
   else
       kit_image.setImage(juce::Image ());

}
*/

void CAudioProcessorEditor::log (const std::string &s)
{
  String v = s + "\n"; 
  log_area.setFont (f_log);
  log_area.insertTextAtCaret (s);
  log_area.setCaretPosition (0);
}


void CAudioProcessorEditor::update_kits_list()
{
  drumkits_model.items.clear();
  
  for (size_t i = 0; i < audioProcessor.scanner.v_kits_names.size(); i++)
      {
       drumkits_model.items.push_back (audioProcessor.scanner.v_kits_names[i]);
       drumkits_model.indexes.push_back (i);
      }
}


void CAudioProcessorEditor::adapt()
{
  if (! audioProcessor.drumkit)
     return;
    
  if (audioProcessor.drumkit->kit_type != KIT_TYPE_DRUMLABOOH)
    {
     log ("Cannot adapt the non-Drumlabooh kit"); 
     return;
    } 
  
  std::string new_path = get_home_dir() + "/drum_sklad/";
  std::string srate = std::to_string (audioProcessor.session_samplerate);
  
  new_path += audioProcessor.drumkit->kit_name;
  new_path += "-";
  new_path += srate;
  
  File source_dir (audioProcessor.drumkit->kit_dir);
  File dest_dir (new_path);
  
  if (source_dir.copyDirectoryTo (dest_dir)) 
     {
      audioProcessor.scanner.scan(); 
      update_kits_list();
      
      audioProcessor.drumkit_path = new_path + "/drumkit.txt";
      
      tmr_leds.stopTimer();

      audioProcessor.load_kit (audioProcessor.drumkit_path);

      //update GUI
      audioProcessor.drumkit->save();
           
      load_kit();
               
      log (audioProcessor.drumkit->kit_name);
      log (bytes_to_file_size (audioProcessor.drumkit->total_samples_size()));
                                     
      tmr_leds.startTimer (1000 / 15); //15 FPS
     }
}


CAudioProcessorEditor::CAudioProcessorEditor (CAudioProcessor &parent, juce::AudioProcessorValueTreeState &vts)
                                             : AudioProcessorEditor (&parent),
                                               audioProcessor (parent),
                                               valueTreeState (vts)
{
  getLookAndFeel().setColour (TextButton::ColourIds::buttonColourId, juce::Colour (87, 110, 113));

  drumkits_model.editor = this;
  
  need_to_update_cells = true;
  
  //kits_scanner.scan();
/*
  for (size_t i = 0; i < audioProcessor.scanner.v_kits_names.size(); i++)
      {
//       cmb_drumkit_selector.addItem (kits_scanner.v_kits_names[i], i + 1);
       
       drumkits_model.items.push_back (audioProcessor.scanner.v_kits_names[i]);
       drumkits_model.indexes.push_back (i);
      }
*/
  update_kits_list();

  //NamedValueSet& drumkits_listbox_props = drumkits_listbox.getProperties();   
  //drumkits_listbox_props.set ("fontSize", 24);
      
  drumkits_listbox.setRowHeight (32);
  drumkits_listbox.setModel (&drumkits_model);    
      
  //int yoffs = YFILLER * 2 ;
  //int xoffs = XFILLER * 2;
  
  
//TOP SECTION
  
  addAndMakeVisible (gr_topbar);
  
  std::string PRODUCT = "DRUMLABOOH " + std::string (VERSION_NUMBER);
    
  addAndMakeVisible (l_plugin_name);
  l_plugin_name.setSize (280 + XFILLER, 36);
  l_plugin_name.setTopLeftPosition (XFILLER, YFILLER);
  //l_plugin_name.setText (juce::CharPointer_UTF8("DRUMLABOOH"), juce::NotificationType::dontSendNotification);
  l_plugin_name.setFont	(f_logo);
  l_plugin_name.setText (PRODUCT.c_str(), juce::NotificationType::dontSendNotification);

  addAndMakeVisible (l_plugin_author);
  l_plugin_author.setSize (100, 24);
  l_plugin_author.setTopLeftPosition (l_plugin_name.getRight(), YFILLER);
  l_plugin_author.setText (juce::CharPointer_UTF8("by Peter Semiletov"), juce::NotificationType::dontSendNotification);
  //l_plugin_author.setFont (f_logo);

  addAndMakeVisible (hl_homepage);
  hl_homepage.setSize (280 + XFILLER, 24);
  hl_homepage.setTopLeftPosition (l_plugin_author.getRight() + XFILLER, YFILLER);
  hl_homepage.setURL (juce::URL("https://psemiletov.github.io/drumlabooh"));
  hl_homepage.setButtonText ("psemiletov.github.io/drumlabooh");
  hl_homepage.setFont (f_sitelink, false, juce::Justification::left);
  hl_homepage.setColour (juce::HyperlinkButton::textColourId, juce::Colour (121, 164, 103));
  //make link color 121 164 103
  
  gr_topbar.setSize (hl_homepage.getRight() + XFILLER * 2, l_plugin_name.getHeight() + YFILLER);
  gr_topbar.setTopLeftPosition (1, 1);

  
//TOP SECTION END
  
  
//LEFT SECTION  
  
  
//DRUMKITS SELECTOR    
  addAndMakeVisible (gr_drumkits);

  gr_drumkits.setTopLeftPosition (gr_topbar.getX(), gr_topbar.getBottom());
  //gr_drumkits.setSize (280 + XFILLER + XFILLER, 240 + YFILLER + YFILLER);
 
  addAndMakeVisible (drumkits_listbox);
  
  drumkits_listbox.setSize (280, 240);
  drumkits_listbox.setTopLeftPosition (gr_drumkits.getX() + XFILLER * 2, gr_drumkits.getY() + YFILLER);
    
  gr_drumkits.setSize (drumkits_listbox.getWidth() + XFILLER * 4, drumkits_listbox.getHeight() + YFILLER * 2);

  

  //KIT INFO

  addAndMakeVisible (gr_kitinfo);
  gr_kitinfo.setTopLeftPosition (gr_drumkits.getX(), gr_drumkits.getBottom());
  gr_kitinfo.setSize (gr_drumkits.getWidth(), 200 + YFILLER); 
  
  addAndMakeVisible (l_kitinfo);
  l_kitinfo.setFont (f_kitname_font);
  l_kitinfo.setTopLeftPosition (gr_kitinfo.getX() + XFILLER, gr_kitinfo.getY());
  l_kitinfo.setSize (280, 48);

  addAndMakeVisible (kit_image);
  kit_image.setSize (gr_kitinfo.getWidth() - XFILLER * 3, 200 - YFILLER * 3);
  kit_image.setCentrePosition ((gr_kitinfo.getX() + gr_kitinfo.getWidth() / 2), (gr_kitinfo.getY() + gr_kitinfo.getHeight() / 2));

  
  
/*  
  cmb_drumkit_selector.setText ("CLICK HERE TO SELECT THE DRUMKIT", juce::NotificationType::dontSendNotification);
  cmb_drumkit_selector.setScrollWheelEnabled (true);
  cmb_drumkit_selector.setTooltip ("Select from installed drum kits");

  cmb_drumkit_selector.setColour (juce::ComboBox::backgroundColourId, juce::Colour (87, 110, 113));
  cmb_drumkit_selector.setColour (juce::ComboBox::textColourId, Colours::white);

  addAndMakeVisible (cmb_drumkit_selector);

  cmb_drumkit_selector.addListener (this);
  cmb_drumkit_selector.setTopLeftPosition (xoffs, 8);
*/

//BELOW KIT INFO
//SOME BUTTONS

  addAndMakeVisible (gr_kitbuttons);
  gr_kitbuttons.setSize (gr_kitinfo.getWidth(), 48 + YFILLER); 
  gr_kitbuttons.setTopLeftPosition (gr_kitinfo.getX(), gr_kitinfo.getBottom());
  
  
  bt_file_open.setButtonText ("OPEN");
  addAndMakeVisible (bt_file_open);

  bt_file_open.setTopLeftPosition (gr_kitbuttons.getX() + XFILLER, gr_kitbuttons.getY() + YFILLER);
  bt_file_open.setSize (54, 40);
    
  bt_file_open.onClick = [this] {

                                 dlg_fileopen = std::make_unique<juce::FileChooser> ("Select file to load...",
                                                File::getSpecialLocation (juce::File::userHomeDirectory));

                                 auto folderChooserFlags = juce::FileBrowserComponent::openMode;

                                 dlg_fileopen->launchAsync (folderChooserFlags, [this] (const juce::FileChooser& chooser)
                                    {
                                     juce::File f (dlg_fileopen->getResult());
                                     if (! f.exists())
                                        return;


                                     std::string full (f.getFullPathName().toRawUTF8());

                                     bool supported = false;

                                     if (ends_with (full, ".xml"))
                                        supported = true;

                                     if (ends_with (full, ".sfz"))
                                        supported = true;

                                     if (ends_with (full, "drumkit.txt"))
                                        supported = true;

                                     if (! supported)
                                        return;

                                     audioProcessor.drumkit_path = full;
                             
                                     tmr_leds.stopTimer();

                                     audioProcessor.load_kit (full);

                                      //update GUI
  
//                                     load_kit();
                                     
                                     need_to_update_cells = false;
                                     
                                     log (audioProcessor.drumkit->kit_name);
                                     log (bytes_to_file_size (audioProcessor.drumkit->total_samples_size()));
                                     
                                     tmr_leds.startTimer (1000 / 15); //15 FPS
                                    });

                            };

                            
  bt_kit_adapt.setButtonText ("ADAPT");
  addAndMakeVisible (bt_kit_adapt);

  bt_kit_adapt.setTopLeftPosition (bt_file_open.getRight() + XFILLER, gr_kitbuttons.getY() + YFILLER);
  bt_kit_adapt.setSize (54, 40);
    
  bt_kit_adapt.onClick = [this] { 
                                 adapt(); 

  };                             
                            
//END KIT BUTTONS
/////////////////////////////////                            
                       
                                          
                            
//RIGHT AREA                           
                            
//DRUMCELLS      
                            
                            
//  yoffs += 52;

  drumcells_container.setSize (drumcells[0].getWidth() + XFILLER, drumcells[0].getHeight() * 36);

  for (size_t i = 0; i < 36; i++)
      {
       drumcells[i].setTopLeftPosition (0, i * drumcells[i].getHeight());
       drumcells_container.addAndMakeVisible (drumcells[i]);
       drumcells[i].attach_params (this, i);
      }


//  load_kit (audioProcessor.drumkit_path);
  

  addAndMakeVisible (drumcells_group);

  drumcells_viewer.setViewedComponent (&drumcells_container, false);
  drumcells_viewer.setScrollBarsShown (true, false);
//  drumcells_viewer.setSize (drumcells_container.getWidth() + (XFILLER * 5), 600);
  drumcells_viewer.setSize (drumcells_container.getWidth() + (XFILLER * 5), 525);

  drumcells_viewer.setScrollBarThickness (24);

  drumcells_group.setTopLeftPosition (gr_drumkits.getRight() + XFILLER, gr_topbar.getBottom());

  
//  drumcells_group.setSize (drumcells_viewer.getWidth() + (XFILLER * 3), 699 + (YFILLER * 2));
//  drumcells_group.setSize (drumcells_viewer.getWidth() + (XFILLER * 3), gr_kitbuttons.getBottom());
  drumcells_group.setSize (drumcells_viewer.getWidth() + (XFILLER * 3), 550);


  drumcells_viewer.setTopLeftPosition (drumcells_group.getX() + XFILLER, drumcells_group.getY() + YFILLER);

  //cmb_drumkit_selector.setSize (drumcells_group.getWidth() - (64 + XFILLER), 48);

  addAndMakeVisible (drumcells_viewer);

/////////////////////////  
//DRUMCELLS END  
  

  
////////////OPTIONS
  
  addAndMakeVisible (gr_options);
  gr_options.setTopLeftPosition (gr_kitbuttons.getX(), gr_kitbuttons.getBottom());

  
  
  addAndMakeVisible (l_midimap_mode);
  l_midimap_mode.setTopLeftPosition (gr_options.getX() + XFILLER, gr_options.getY() + YFILLER);
  l_midimap_mode.setSize (120, 32);
  cmb_midimap_mode.setColour (juce::ComboBox::backgroundColourId, juce::Colour (87, 110, 113));
  cmb_midimap_mode.setColour (juce::ComboBox::textColourId, Colours::white);
  
 
  addAndMakeVisible (cmb_midimap_mode);
  cmb_midimap_mode.setSize (180 + XFILLER, 42);
  cmb_midimap_mode.setTopLeftPosition (l_midimap_mode.getX() + l_midimap_mode.getWidth(),
                                       l_midimap_mode.getY());
  
  cmb_midimap_mode.addItem ("Automatic", MIDIMAPMODE_LABOOH); 
  cmb_midimap_mode.addItem ("Kit-defined", MIDIMAPMODE_FROMKIT);
  
  att_midimap_mode.reset (new juce::AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, "midimap_mode", cmb_midimap_mode));
 

  
  addAndMakeVisible (l_base_note);
  l_base_note.setSize (120, 48);
  l_base_note.setTopLeftPosition (gr_options.getX() + XFILLER, cmb_midimap_mode.getBottom() + YFILLER);
  
  
  l_base_note.setTooltip ("Number of MIDI note from which\n we start to map instruments, \n default 36");

  addAndMakeVisible (sl_base_note);
  sl_base_note.setSliderStyle (juce::Slider::SliderStyle::IncDecButtons);
  sl_base_note.setTopLeftPosition (l_base_note.getRight() + XFILLER, l_base_note.getY());
  sl_base_note.setSize (120, 48);
  sl_base_note.setRange (0, 127, 1.0);
  sl_base_note.setValue (audioProcessor.base_note_number, dontSendNotification);
  sl_base_note.addListener (this);
  sl_base_note.setTooltip ("Number of MIDI note from which\n we start to map instruments, \n default 36");

   
  
  addAndMakeVisible (bt_ignore_midi_velocity);
  
  att_ignore_midi_velocity.reset (new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, "ignore_midi_velocity", bt_ignore_midi_velocity));
  bt_ignore_midi_velocity.setButtonText ("Ignore MIDI velocity");
  bt_ignore_midi_velocity.setTooltip ("If turned on, play first layer\n of multi-layered samples,\n and with the maximun velocity");
  bt_ignore_midi_velocity.setSize (180 + XFILLER, 48);
  bt_ignore_midi_velocity.setTopLeftPosition (l_base_note.getX(), sl_base_note.getBottom());
  
  
 

#ifndef MULTICHANNEL
  
    // PAN MODE
  
  addAndMakeVisible (l_pan_mode);
//  l_pan_mode.setTopLeftPosition (bt_global_analog_on.getX(), bt_global_analog_on.getBottom() + YFILLER);
  
  
  l_pan_mode.setTopLeftPosition (cmb_midimap_mode.getRight() + XFILLER * 4, 
                                 l_midimap_mode.getY());

  
  l_pan_mode.setSize (100, 32);
  cmb_pan_mode.setColour (juce::ComboBox::backgroundColourId, juce::Colour (87, 110, 113));
  cmb_pan_mode.setColour (juce::ComboBox::textColourId, Colours::white);

  addAndMakeVisible (cmb_pan_mode);
      //  cmb_pan_mode.onChange = [this] { panner_modeMenuChanged(); };
  cmb_pan_mode.setSize (180 + XFILLER, 42);
  
  
  cmb_pan_mode.addItem ("equal power panning, law: -3 dB", PANMODE01); //sin/cos panner
  cmb_pan_mode.addItem ("square root panner, law: -3 dB", PANMODE02);
  cmb_pan_mode.addItem ("linear panner, law: 0 dB", PANMODE03);
  cmb_pan_mode.addItem ("linear panner, law: -6 dB", PANMODE04);
  cmb_pan_mode.addItem ("power panner, law: -4.5 dB", PANMODE05);
  cmb_pan_mode.addItem ("power panner, law: -6 dB", PANMODE06);
  cmb_pan_mode.addItem ("sinus panner, law: -1.3 dB", PANMODE07);

  att_pan_mode.reset (new juce::AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, "panner_mode", cmb_pan_mode));

  cmb_pan_mode.setTopLeftPosition (l_pan_mode.getRight(), l_pan_mode.getY());
  
  
  
  //////////////NEW GLOBAL ANALOG
  addAndMakeVisible (bt_global_analog_on);
  bt_global_analog_on.setButtonText ("Analog");
  //bt_ignore_midi_velocity.setTooltip ("If turned on, play first layer\n of multi-layered samples,\n and with the maximun velocity");
  bt_global_analog_on.setSize (80, 48);
  bt_global_analog_on.setTopLeftPosition (l_pan_mode.getX(), 
                                          l_pan_mode.getBottom() + YFILLER);

  att_global_analog_on.reset (new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, "global_analog_on", bt_global_analog_on));
  
 
  addAndMakeVisible (sl_global_analog_amount);
  sl_global_analog_amount.setTopLeftPosition (cmb_pan_mode.getX(), bt_global_analog_on.getY() + YFILLER);
  sl_global_analog_amount.setSize (192, 48);
  sl_global_analog_amount.setRange (0.0f, 1.0f, 0.01f);
  sl_global_analog_amount.setSliderStyle (juce::Slider::LinearHorizontal);
  sl_global_analog_amount.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
  sl_global_analog_amount.setColour (juce::Slider::thumbColourId, juce::Colours::orange);
  
  att_global_analog_amount.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "global_analog_amount", sl_global_analog_amount));
   

  
  addAndMakeVisible (log_area);
  log_area.setMultiLine (true, true);
  log_area.setReadOnly (true);
   log_area.setTopLeftPosition (sl_global_analog_amount.getRight(), l_midimap_mode.getY());
  log_area.setSize (170, 148);
  
  
#endif
  
  

#ifndef MULTICHANNEL
  
  
  gr_options.setSize (810, sl_base_note.getHeight() + 
                           cmb_pan_mode.getHeight() + 
                           cmb_midimap_mode.getHeight() + 
                           bt_ignore_midi_velocity.getHeight());
#else

  
  addAndMakeVisible (log_area);
  log_area.setFont (f_log);
  log_area.setTopLeftPosition (cmb_midimap_mode.getRight(), l_midimap_mode.getY());
  log_area.setSize (170, 148);

  
  gr_options.setSize (810, sl_base_note.getHeight() + 
                           bt_ignore_midi_velocity.getHeight());
  
#endif  

  
///////////////END OF ANALOG  
   
  
  
////////////////////////              
  
//  setSize (drumcells_viewer.getRight() + XFILLER * 2, 
  //         gr_options.getBottom() + YFILLER * 2);

   setSize (drumcells_viewer.getRight() + XFILLER * 2, 
           gr_options.getBottom());

 if (audioProcessor.drumkit)
    {
     log (audioProcessor.drumkit->kit_name);
     log (bytes_to_file_size (audioProcessor.drumkit->total_samples_size()));
    }

  //load_kit();

  
  for (size_t i = 0; i < 36; i++)
      {
       drumcells[i].set_name ("EMPTY CELL");
       drumcells[i].cell_label.setColour (juce::Label::backgroundColourId, juce::Colour (131, 152, 160));
      }

  
  
 
  tmr_leds.uplink = this;
  tmr_leds.startTimer (1000 / 15); //15 FPS

//bt_test.setBounds (drumcells[0].getWidth() + 50, 400, 50, 50);
}


CAudioProcessorEditor::~CAudioProcessorEditor()
{
  att_ignore_midi_velocity = nullptr;
  att_base_note = nullptr;
  
#ifndef MULTICHANNEL

  att_pan_mode = nullptr;
  att_global_analog_amount = nullptr;
  att_global_analog_on = nullptr;

#endif
}

//==============================================================================
void CAudioProcessorEditor::paint (juce::Graphics& g)
{

//        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
  g.fillAll (juce::Colour::fromRGB (103, 97, 114)	);
}


void CAudioProcessorEditor::resized()
{

}


void CAudioProcessorEditor::buttonClicked (juce::Button* button)
{
 /* if (button == &bt_test)
    {
     std::cout << "NewProjectAudioProcessorEditor::buttonClicked " << std::endl;
  }*/
}


void CAudioProcessorEditor::kit_changed()
{
 //(cmb_drumkit_selector.getSelectedId()
//  std::cout << cmb_drumkit_selector.getSelectedId() - 1<< std::endl;
}


void CAudioProcessorEditor::comboBoxChanged (juce::ComboBox *comboBox)
{
   //std::cout << "CAudioProcessorEditor::comboBoxChanged" << std::endl;
/*  if (comboBox == &cmb_drumkit_selector)
     {
      int id = cmb_drumkit_selector.getSelectedId();
      if (id == 0)
         return;

      std::string full = kits_scanner.map_kits[kits_scanner.v_kits_names [cmb_drumkit_selector.getSelectedId() - 1]];

      audioProcessor.drumkit_path = full;
      tmr_leds.stopTimer();

      audioProcessor.load_kit (full);

      //update GUI
      load_kit (full);

      
      if (audioProcessor.drumkit)
      {log (audioProcessor.drumkit->kit_name);
      log (bytes_to_file_size (audioProcessor.drumkit->total_samples_size()));
      }
      
      tmr_leds.startTimer (1000 / 15); //15 FPS
    }
    */
}


void CAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
  if (slider == &sl_base_note)
     {
      audioProcessor.base_note_number = sl_base_note.getValue();
     }
}


/*
void CToggleButton.paintButton 	( 	Graphics &  	g,
		bool  	shouldDrawButtonAsHighlighted,
		bool  	shouldDrawButtonAsDown
	)
{


}
*/


void CTimer::hiResTimerCallback()
{
  if (! uplink)
      return;

  if (! uplink->isVisible())
     return;

  if (! uplink->audioProcessor.drumkit)
     return;

  if (uplink->need_to_update_cells && uplink->audioProcessor.drumkit->loaded)
     {
      uplink->need_to_update_cells = false;
      uplink->load_kit();
     }
  
  for (int i = 0; i < uplink->audioProcessor.drumkit->v_samples.size(); i++)
      {
       bool actv = uplink->audioProcessor.drumkit->v_samples[i]->active;
       uplink->drumcells[i].led.is_on = actv;
       uplink->drumcells[i].led.velocity = uplink->audioProcessor.drumkit->v_samples[i]->velocity;
       uplink->drumcells[i].led.repaint();
      }
}




CDrumkitsListBoxModel::CDrumkitsListBoxModel()
{
//	JgtGetBoards(); //builds board array and sets numBoards
  editor = 0;

}

CDrumkitsListBoxModel::~CDrumkitsListBoxModel()
{

  
}


int CDrumkitsListBoxModel::getNumRows()
{
	return items.size();
}


void CDrumkitsListBoxModel::paintListBoxItem (int rowNumber, Graphics &g, int width, int height, bool rowIsSelected)
{
  if (rowIsSelected)
	  g.fillAll (Colours::cornflowerblue);
 	  //g.fillAll (Colours::lightblue);
 
  else
	  //g.fillAll (Colours::cornflowerblue);
    //g.fillAll (Colours::plum);
    g.fillAll (Colours::paleturquoise);
    
  g.setFont (20); 

 g.drawText (items[rowNumber].c_str(),
		        4, 0, width - 4, height,
		        Justification::centredLeft, true);

 
  //g.drawFittedText (items[rowNumber].c_str(),
	//	        4, 0, width - 4, height,
		//        Justification::centredLeft, 2);
}


void CDrumkitsListBoxModel::selectedRowsChanged (int lastRowSelected)
{
  if (! editor)
      return;
  
  //std::string full = editor->kits_scanner.map_kits[kits_scanner.v_kits_names [cmb_drumkit_selector.getSelectedId() - 1]];
  std::string full = editor->audioProcessor.scanner.map_kits[editor->audioProcessor.scanner.v_kits_names [lastRowSelected]]; //why not direct index?

  editor->audioProcessor.drumkit_path = full;
  editor->tmr_leds.stopTimer();

  editor->audioProcessor.load_kit (full);

  editor->tmr_leds.startTimer (1000 / 15); //15 FPS  

      //update GUI
// editor->load_kit (full);

  if (editor->audioProcessor.drumkit)
     {
      editor->load_kit();
        
      editor->log ("***\n");
      editor->log ("\n");
            editor->log (bytes_to_file_size (editor->audioProcessor.drumkit->total_samples_size()));
      editor->log ("\n");
      editor->log (editor->audioProcessor.drumkit->kit_name);
      editor->log ("loaded:\n");
     }

}
