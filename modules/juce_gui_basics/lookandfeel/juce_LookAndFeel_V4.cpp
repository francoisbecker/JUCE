/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 5 End-User License
   Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.juce.com/juce-5-licence
   Privacy Policy: www.juce.com/juce-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

Colour LookAndFeel_V4::ColourScheme::getUIColour (UIColour index) const noexcept
{
    if (isPositiveAndBelow (index, numColours))
        return palette[index];

    jassertfalse;
    return {};
}

void LookAndFeel_V4::ColourScheme::setUIColour (UIColour index, Colour newColour) noexcept
{
    if (isPositiveAndBelow (index, numColours))
        palette[index] = newColour;
    else
        jassertfalse;
}

bool LookAndFeel_V4::ColourScheme::operator== (const ColourScheme& other) const noexcept
{
    for (int i = 0; i < numColours; ++i)
        if (palette[i] != other.palette[i])
            return false;

    return true;
}

bool LookAndFeel_V4::ColourScheme::operator!= (const ColourScheme& other) const noexcept
{
    return ! operator== (other);
}

//==============================================================================
LookAndFeel_V4::LookAndFeel_V4()  : currentColourScheme (getDarkColourScheme())
{
    initialiseColours();
}

LookAndFeel_V4::LookAndFeel_V4 (ColourScheme scheme)  : currentColourScheme (scheme)
{
    initialiseColours();
}

LookAndFeel_V4::~LookAndFeel_V4()  {}

//==============================================================================
void LookAndFeel_V4::setColourScheme (ColourScheme newColourScheme)
{
    currentColourScheme = newColourScheme;
    initialiseColours();
}

LookAndFeel_V4::ColourScheme LookAndFeel_V4::getDarkColourScheme()
{
    return { 0xff323e44, 0xff263238, 0xff323e44,
             0xff8e989b, 0xffffffff, 0xff42a2c8,
             0xffffffff, 0xff181f22, 0xffffffff };
}

LookAndFeel_V4::ColourScheme LookAndFeel_V4::getMidnightColourScheme()
{
    return { 0xff2f2f3a, 0xff191926, 0xffd0d0d0,
             0xff66667c, 0xc8ffffff, 0xffd8d8d8,
             0xffffffff, 0xff606073, 0xff000000 };
}

LookAndFeel_V4::ColourScheme LookAndFeel_V4::getGreyColourScheme()
{
    return { 0xff505050, 0xff424242, 0xff606060,
             0xffa6a6a6, 0xffffffff, 0xff21ba90,
             0xff000000, 0xffffffff, 0xffffffff };
}

LookAndFeel_V4::ColourScheme LookAndFeel_V4::getLightColourScheme()
{
    return { 0xffefefef, 0xffffffff, 0xffffffff,
             0xffdddddd, 0xff000000, 0xffa9a9a9,
             0xffffffff, 0xff42a2c8, 0xff000000 };
}

//==============================================================================
class LookAndFeel_V4_DocumentWindowButton   : public Button
{
public:
    LookAndFeel_V4_DocumentWindowButton (const String& name, Colour c, const Path& normal, const Path& toggled)
        : Button (name), colour (c), normalShape (normal), toggledShape (toggled)
    {
    }

    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown) override
    {
        auto background = Colours::grey;

        if (auto* rw = findParentComponentOfClass<ResizableWindow>())
            if (auto lf = dynamic_cast<LookAndFeel_V4*> (&rw->getLookAndFeel()))
                background = lf->getCurrentColourScheme().getUIColour (LookAndFeel_V4::ColourScheme::widgetBackground);

        g.fillAll (background);

        g.setColour ((! isEnabled() || isButtonDown) ? colour.withAlpha (0.6f)
                                                     : colour);

        if (isMouseOverButton)
        {
            g.fillAll();
            g.setColour (background);
        }

        auto& p = getToggleState() ? toggledShape : normalShape;

        auto reducedRect = Justification (Justification::centred)
                              .appliedToRectangle (Rectangle<int> (getHeight(), getHeight()), getLocalBounds())
                              .toFloat()
                              .reduced (getHeight() * 0.3f);

        g.fillPath (p, p.getTransformToScaleToFit (reducedRect, true));
    }

private:
    Colour colour;
    Path normalShape, toggledShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LookAndFeel_V4_DocumentWindowButton)
};

Button* LookAndFeel_V4::createDocumentWindowButton (int buttonType)
{
    Path shape;
    const float crossThickness = 0.15f;

    if (buttonType == DocumentWindow::closeButton)
    {
        shape.addLineSegment ({ 0.0f, 0.0f, 1.0f, 1.0f }, crossThickness);
        shape.addLineSegment ({ 1.0f, 0.0f, 0.0f, 1.0f }, crossThickness);

        return new LookAndFeel_V4_DocumentWindowButton ("close", Colour (0xff9A131D), shape, shape);
    }

    if (buttonType == DocumentWindow::minimiseButton)
    {
        shape.addLineSegment ({ 0.0f, 0.5f, 1.0f, 0.5f }, crossThickness);

        return new LookAndFeel_V4_DocumentWindowButton ("minimise", Colour (0xffaa8811), shape, shape);
    }

    if (buttonType == DocumentWindow::maximiseButton)
    {
        shape.addLineSegment ({ 0.5f, 0.0f, 0.5f, 1.0f }, crossThickness);
        shape.addLineSegment ({ 0.0f, 0.5f, 1.0f, 0.5f }, crossThickness);

        Path fullscreenShape;
        fullscreenShape.startNewSubPath (45.0f, 100.0f);
        fullscreenShape.lineTo (0.0f, 100.0f);
        fullscreenShape.lineTo (0.0f, 0.0f);
        fullscreenShape.lineTo (100.0f, 0.0f);
        fullscreenShape.lineTo (100.0f, 45.0f);
        fullscreenShape.addRectangle (45.0f, 45.0f, 100.0f, 100.0f);
        PathStrokeType (30.0f).createStrokedPath (fullscreenShape, fullscreenShape);

        return new LookAndFeel_V4_DocumentWindowButton ("maximise", Colour (0xff0A830A), shape, fullscreenShape);
    }

    jassertfalse;
    return nullptr;
}

void LookAndFeel_V4::positionDocumentWindowButtons (DocumentWindow&,
                                                    int titleBarX, int titleBarY,
                                                    int titleBarW, int titleBarH,
                                                    Button* minimiseButton,
                                                    Button* maximiseButton,
                                                    Button* closeButton,
                                                    bool positionTitleBarButtonsOnLeft)
{
    titleBarH = jmin (titleBarH, titleBarH - titleBarY);

    const int buttonW = (int) (titleBarH * 1.2);

    int x = positionTitleBarButtonsOnLeft ? titleBarX
                                          : titleBarX + titleBarW - buttonW;

    if (closeButton != nullptr)
    {
        closeButton->setBounds (x, titleBarY, buttonW, titleBarH);
        x += positionTitleBarButtonsOnLeft ? buttonW : -buttonW;
    }

    if (positionTitleBarButtonsOnLeft)
        std::swap (minimiseButton, maximiseButton);

    if (maximiseButton != nullptr)
    {
        maximiseButton->setBounds (x, titleBarY, buttonW, titleBarH);
        x += positionTitleBarButtonsOnLeft ? buttonW : -buttonW;
    }

    if (minimiseButton != nullptr)
        minimiseButton->setBounds (x, titleBarY, buttonW, titleBarH);
}

void LookAndFeel_V4::drawDocumentWindowTitleBar (DocumentWindow& window, Graphics& g,
                                                 int w, int h, int titleSpaceX, int titleSpaceW,
                                                 const Image* icon, bool drawTitleTextOnLeft)
{
    if (w * h == 0)
        return;

    const bool isActive = window.isActiveWindow();

    g.setColour (getCurrentColourScheme().getUIColour (ColourScheme::widgetBackground));
    g.fillAll();

    Font font (h * 0.65f, Font::plain);
    g.setFont (font);

    int textW = font.getStringWidth (window.getName());
    int iconW = 0;
    int iconH = 0;

    if (icon != nullptr)
    {
        iconH = (int) font.getHeight();
        iconW = icon->getWidth() * iconH / icon->getHeight() + 4;
    }

    textW = jmin (titleSpaceW, textW + iconW);
    int textX = drawTitleTextOnLeft ? titleSpaceX
                                    : jmax (titleSpaceX, (w - textW) / 2);

    if (textX + textW > titleSpaceX + titleSpaceW)
        textX = titleSpaceX + titleSpaceW - textW;

    if (icon != nullptr)
    {
        g.setOpacity (isActive ? 1.0f : 0.6f);
        g.drawImageWithin (*icon, textX, (h - iconH) / 2, iconW, iconH,
                           RectanglePlacement::centred, false);
        textX += iconW;
        textW -= iconW;
    }

    if (window.isColourSpecified (DocumentWindow::textColourId) || isColourSpecified (DocumentWindow::textColourId))
        g.setColour (window.findColour (DocumentWindow::textColourId));
    else
        g.setColour (getCurrentColourScheme().getUIColour (ColourScheme::defaultText));

    g.drawText (window.getName(), textX, 0, textW, h, Justification::centredLeft, true);
}

//==============================================================================
void LookAndFeel_V4::drawButtonBackground (Graphics& g,
                                           Button& button,
                                           const Colour& backgroundColour,
                                           bool isMouseOverButton,
                                           bool isButtonDown)
{
    const auto cornerSize = 6.0f;
    const auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

    auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                      .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

    if (isButtonDown || isMouseOverButton)
        baseColour = baseColour.contrasting (isButtonDown ? 0.2f : 0.05f);

    g.setColour (baseColour);

    if (button.isConnectedOnLeft() || button.isConnectedOnRight())
    {
        Path path;
        path.addRoundedRectangle (bounds.getX(), bounds.getY(),
                                  bounds.getWidth(), bounds.getHeight(),
                                  cornerSize, cornerSize,
                                  ! button.isConnectedOnLeft(),
                                  ! button.isConnectedOnRight(),
                                  ! button.isConnectedOnLeft(),
                                  ! button.isConnectedOnRight());

        g.fillPath (path);

        g.setColour (button.findColour (ComboBox::outlineColourId));
        g.strokePath (path, PathStrokeType (1.0f));
    }
    else
    {
        g.fillRoundedRectangle (bounds, cornerSize);

        g.setColour (button.findColour (ComboBox::outlineColourId));
        g.drawRoundedRectangle (bounds, cornerSize, 1.0f);
    }
}

void LookAndFeel_V4::drawToggleButton (Graphics& g, ToggleButton& button,
                                       bool isMouseOverButton, bool isButtonDown)
{
    const auto fontSize = jmin (15.0f, button.getHeight() * 0.75f);
    const auto tickWidth = fontSize * 1.1f;

    drawTickBox (g, button, 4.0f, (button.getHeight() - tickWidth) * 0.5f,
                 tickWidth, tickWidth,
                 button.getToggleState(),
                 button.isEnabled(),
                 isMouseOverButton,
                 isButtonDown);

    g.setColour (button.findColour (ToggleButton::textColourId));
    g.setFont (fontSize);

    if (! button.isEnabled())
        g.setOpacity (0.5f);

    g.drawFittedText (button.getButtonText(),
                      button.getLocalBounds().withTrimmedLeft (roundToInt (tickWidth) + 10)
                                             .withTrimmedRight (2),
                      Justification::centredLeft, 10);
}

void LookAndFeel_V4::drawTickBox (Graphics& g, Component& component,
                                  float x, float y, float w, float h,
                                  const bool ticked,
                                  const bool isEnabled,
                                  const bool isMouseOverButton,
                                  const bool isButtonDown)
{
    ignoreUnused (isEnabled, isMouseOverButton, isButtonDown);

    Rectangle<float> tickBounds (x, y, w, h);

    g.setColour (component.findColour (ToggleButton::tickDisabledColourId));
    g.drawRoundedRectangle (tickBounds, 4.0f, 1.0f);

    if (ticked)
    {
        g.setColour (component.findColour (ToggleButton::tickColourId));
        const auto tick = getTickShape (0.75f);
        g.fillPath (tick, tick.getTransformToScaleToFit (tickBounds.reduced (4, 5).toFloat(), false));
    }
}

//==============================================================================
AlertWindow* LookAndFeel_V4::createAlertWindow (const String& title, const String& message,
                                                const String& button1, const String& button2, const String& button3,
                                                AlertWindow::AlertIconType iconType,
                                                int numButtons, Component* associatedComponent)
{
    const auto boundsOffset = 50;

    auto* aw = LookAndFeel_V2::createAlertWindow (title, message, button1, button2, button3,
                                                  iconType, numButtons, associatedComponent);

    auto bounds = aw->getBounds();
    bounds = bounds.withSizeKeepingCentre (bounds.getWidth() + boundsOffset, bounds.getHeight() + boundsOffset);
    aw->setBounds (bounds);

    for (auto* child : aw->getChildren())
        if (auto button = dynamic_cast<TextButton*> (child))
            button->setBounds (button->getBounds() + Point<int> (25, 40));

    return aw;
}

void LookAndFeel_V4::drawAlertBox (Graphics& g, AlertWindow& alert,
                                   const Rectangle<int>& textArea, TextLayout& textLayout)
{
    const auto cornerSize = 4.0f;

    g.setColour (alert.findColour (AlertWindow::outlineColourId));
    g.drawRoundedRectangle (alert.getLocalBounds().toFloat(), cornerSize, 2.0f);

    const auto bounds = alert.getLocalBounds().reduced (1);
    g.reduceClipRegion (bounds);

    g.setColour (alert.findColour (AlertWindow::backgroundColourId));
    g.fillRoundedRectangle (bounds.toFloat(), cornerSize);

    auto iconSpaceUsed = 0;

    const auto iconWidth = 80;
    auto iconSize = jmin (iconWidth + 50, bounds.getHeight() + 20);

    if (alert.containsAnyExtraComponents() || alert.getNumButtons() > 2)
        iconSize = jmin (iconSize, textArea.getHeight() + 50);

    const Rectangle<int> iconRect (iconSize / -10, iconSize / -10,
                                   iconSize, iconSize);

    if (alert.getAlertType() != AlertWindow::NoIcon)
    {
        Path icon;
        char character;
        uint32 colour;

        if (alert.getAlertType() == AlertWindow::WarningIcon)
        {
            character = '!';

            icon.addTriangle (iconRect.getX() + iconRect.getWidth() * 0.5f, (float) iconRect.getY(),
                              (float) iconRect.getRight(), (float) iconRect.getBottom(),
                              (float) iconRect.getX(), (float) iconRect.getBottom());

            icon = icon.createPathWithRoundedCorners (5.0f);
            colour = 0x66ff2a00;
        }
        else
        {
            colour = Colour (0xff00b0b9).withAlpha (0.4f).getARGB();
            character = alert.getAlertType() == AlertWindow::InfoIcon ? 'i' : '?';

            icon.addEllipse (iconRect.toFloat());
        }

        GlyphArrangement ga;
        ga.addFittedText (Font (iconRect.getHeight() * 0.9f, Font::bold),
                          String::charToString ((juce_wchar) (uint8) character),
                          (float) iconRect.getX(), (float) iconRect.getY(),
                          (float) iconRect.getWidth(), (float) iconRect.getHeight(),
                          Justification::centred, false);
        ga.createPath (icon);

        icon.setUsingNonZeroWinding (false);
        g.setColour (Colour (colour));
        g.fillPath (icon);

        iconSpaceUsed = iconSize;
    }

    g.setColour (alert.findColour (AlertWindow::textColourId));

    const Rectangle<int> alertBounds (bounds.getX() + iconSpaceUsed,
                                      30,
                                      bounds.getWidth(),
                                      bounds.getHeight() - getAlertWindowButtonHeight() - 20);

    textLayout.draw (g, alertBounds.toFloat());
}

int LookAndFeel_V4::getAlertWindowButtonHeight()    { return 40; }
Font LookAndFeel_V4::getAlertWindowTitleFont()      { return Font (18.0f, Font::FontStyleFlags::bold); }
Font LookAndFeel_V4::getAlertWindowMessageFont()    { return Font (16.0f); }
Font LookAndFeel_V4::getAlertWindowFont()           { return Font (14.0f); }

//==============================================================================
void LookAndFeel_V4::drawProgressBar (Graphics& g, ProgressBar& progressBar,
                                      int width, int height, double progress, const String& textToShow)
{
    if (width == height)
        drawCircularProgressBar (g, progressBar, textToShow);
    else
        drawLinearProgressBar (g, progressBar, width, height, progress, textToShow);
}

void LookAndFeel_V4::drawLinearProgressBar (Graphics& g, ProgressBar& progressBar,
                                            int width, int height,
                                            double progress, const String& textToShow)
{
    const auto background = progressBar.findColour (ProgressBar::backgroundColourId);
    const auto foreground = progressBar.findColour (ProgressBar::foregroundColourId);

    auto barBounds = progressBar.getLocalBounds().toFloat();

    g.setColour (background);
    g.fillRoundedRectangle (barBounds, progressBar.getHeight() * 0.5f);

    if (progress >= 0.0f && progress <= 1.0f)
    {
        Path p;
        p.addRoundedRectangle (barBounds, progressBar.getHeight() * 0.5f);
        g.reduceClipRegion (p);

        barBounds.setWidth (barBounds.getWidth() * (float) progress);
        g.setColour (foreground);
        g.fillRoundedRectangle (barBounds, progressBar.getHeight() * 0.5f);
    }
    else
    {
        // spinning bar..
        g.setColour (background);

        const auto stripeWidth = height * 2;
        const auto position = (int) (Time::getMillisecondCounter() / 15) % stripeWidth;

        Path p;

        for (auto x = (float) (-position); x < width + stripeWidth; x += stripeWidth)
            p.addQuadrilateral (x, 0.0f,
                                x + stripeWidth * 0.5f, 0.0f,
                                x, (float) height,
                                x - stripeWidth * 0.5f, (float) height);

        Image im (Image::ARGB, width, height, true);

        {
            Graphics g2 (im);
            g2.setColour (foreground);
            g2.fillRoundedRectangle (barBounds, progressBar.getHeight() * 0.5f);
        }

        g.setTiledImageFill (im, 0, 0, 0.85f);
        g.fillPath (p);
    }

    if (textToShow.isNotEmpty())
    {
        g.setColour (Colour::contrasting (background, foreground));
        g.setFont (height * 0.6f);

        g.drawText (textToShow, 0, 0, width, height, Justification::centred, false);
    }
}

void LookAndFeel_V4::drawCircularProgressBar (Graphics& g, ProgressBar& progressBar, const String& progressText)
{
    const auto background = progressBar.findColour (ProgressBar::backgroundColourId);
    const auto foreground = progressBar.findColour (ProgressBar::foregroundColourId);

    auto barBounds = progressBar.getLocalBounds().reduced (2, 2).toFloat();

    auto rotationInDegrees  = static_cast<float> ((Time::getMillisecondCounter() / 10) % 360);
    auto normalisedRotation = rotationInDegrees / 360.0f;

    const auto rotationOffset = 22.5f;
    const auto maxRotation    = 315.0f;

    auto startInDegrees = rotationInDegrees;
    auto endInDegrees   = startInDegrees + rotationOffset;

    if (normalisedRotation >= 0.25f && normalisedRotation < 0.5f)
    {
        const auto rescaledRotation = (normalisedRotation * 4.0f) - 1.0f;
        endInDegrees = startInDegrees + rotationOffset + (maxRotation * rescaledRotation);
    }
    else if (normalisedRotation >= 0.5f && normalisedRotation <= 1.0f)
    {
        endInDegrees = startInDegrees + rotationOffset + maxRotation;
        const auto rescaledRotation = 1.0f - ((normalisedRotation * 2.0f) - 1.0f);
        startInDegrees = endInDegrees - rotationOffset - (maxRotation * rescaledRotation);
    }

    g.setColour (background);
    Path arcPath2;
    arcPath2.addCentredArc (barBounds.getCentreX(),
                            barBounds.getCentreY(),
                            barBounds.getWidth() * 0.5f,
                            barBounds.getHeight() * 0.5f, 0.0f,
                            0.0f,
                            2.0f * float_Pi,
                            true);
    g.strokePath (arcPath2, PathStrokeType (4.0f));

    g.setColour (foreground);
    Path arcPath;
    arcPath.addCentredArc (barBounds.getCentreX(),
                           barBounds.getCentreY(),
                           barBounds.getWidth() * 0.5f,
                           barBounds.getHeight() * 0.5f,
                           0.0f,
                           degreesToRadians (startInDegrees),
                           degreesToRadians (endInDegrees),
                           true);

    arcPath.applyTransform (AffineTransform::rotation (normalisedRotation * float_Pi * 2.25f, barBounds.getCentreX(), barBounds.getCentreY()));
    g.strokePath (arcPath, PathStrokeType (4.0f));

    if (progressText.isNotEmpty())
    {
        g.setColour (progressBar.findColour (TextButton::textColourOffId));
        g.setFont (Font (12.0f, 2));
        g.drawText (progressText, barBounds, Justification::centred, false);
    }
}

//==============================================================================
int LookAndFeel_V4::getDefaultScrollbarWidth()
{
    return 8;
}

void LookAndFeel_V4::drawScrollbar (Graphics& g, ScrollBar& scrollbar, int x, int y, int width, int height,
                                    bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown)
{
    ignoreUnused (isMouseDown);

    Rectangle<int> thumbBounds;

    if (isScrollbarVertical)
        thumbBounds = { x, thumbStartPosition, width, thumbSize };
    else
        thumbBounds = { thumbStartPosition, y, thumbSize, height };

    const auto c = scrollbar.findColour (ScrollBar::ColourIds::thumbColourId);
    g.setColour (isMouseOver ? c.brighter (0.25f) : c);
    g.fillRoundedRectangle (thumbBounds.reduced (1).toFloat(), 4.0f);
}

//==============================================================================
Path LookAndFeel_V4::getTickShape (float height)
{
    static const unsigned char pathData[] = { 110,109,32,210,202,64,126,183,148,64,108,39,244,247,64,245,76,124,64,108,178,131,27,65,246,76,252,64,108,175,242,4,65,246,76,252,
        64,108,236,5,68,65,0,0,160,180,108,240,150,90,65,21,136,52,63,108,48,59,16,65,0,0,32,65,108,32,210,202,64,126,183,148,64, 99,101,0,0 };

    Path path;
    path.loadPathFromData (pathData, sizeof (pathData));
    path.scaleToFit (0, 0, height * 2.0f, height, true);

    return path;
}

Path LookAndFeel_V4::getCrossShape (float height)
{
    static const unsigned char pathData[] = { 110,109,51,51,255,66,0,0,0,0,108,205,204,13,67,51,51,99,65,108,0,0,170,66,205,204,141,66,108,51,179,13,67,52,51,255,66,108,0,0,255,
        66,205,204,13,67,108,205,204,141,66,0,0,170,66,108,52,51,99,65,51,179,13,67,108,0,0,0,0,51,51,255,66,108,205,204,98,66, 204,204,141,66,108,0,0,0,0,51,51,99,65,108,51,51,
        99,65,0,0,0,0,108,205,204,141,66,205,204,98,66,108,51,51,255,66,0,0,0,0,99,101,0,0 };

    Path path;
    path.loadPathFromData (pathData, sizeof (pathData));
    path.scaleToFit (0, 0, height * 2.0f, height, true);

    return path;
}

//==============================================================================
void LookAndFeel_V4::fillTextEditorBackground (Graphics& g, int width, int height, TextEditor& textEditor)
{
    if (dynamic_cast<AlertWindow*> (textEditor.getParentComponent()) != nullptr)
    {
        g.setColour (textEditor.findColour (TextEditor::backgroundColourId));
        g.fillRect (0, 0, width, height);

        g.setColour (textEditor.findColour (TextEditor::outlineColourId));
        g.drawHorizontalLine (height - 1, 0.0f, static_cast<float> (width));
    }
    else
    {
        LookAndFeel_V2::fillTextEditorBackground (g, width, height, textEditor);
    }
}

void LookAndFeel_V4::drawTextEditorOutline (Graphics& g, int width, int height, TextEditor& textEditor)
{
    if (dynamic_cast<AlertWindow*> (textEditor.getParentComponent()) == nullptr)
    {
        if (textEditor.isEnabled())
        {
            if (textEditor.hasKeyboardFocus (true) && ! textEditor.isReadOnly())
            {
                g.setColour (textEditor.findColour (TextEditor::focusedOutlineColourId));
                g.drawRect (0, 0, width, height, 2);
            }
            else
            {
                g.setColour (textEditor.findColour (TextEditor::outlineColourId));
                g.drawRect (0, 0, width, height);
            }
        }
    }
}

//==============================================================================
Button* LookAndFeel_V4::createFileBrowserGoUpButton()
{
    auto* goUpButton = new DrawableButton ("up", DrawableButton::ImageOnButtonBackground);

    Path arrowPath;
    arrowPath.addArrow ({ 50.0f, 100.0f, 50.0f, 0.0f }, 40.0f, 100.0f, 50.0f);

    DrawablePath arrowImage;
    arrowImage.setFill (goUpButton->findColour (TextButton::textColourOffId));
    arrowImage.setPath (arrowPath);

    goUpButton->setImages (&arrowImage);

    return goUpButton;
}

void LookAndFeel_V4::layoutFileBrowserComponent (FileBrowserComponent& browserComp,
                                                 DirectoryContentsDisplayComponent* fileListComponent,
                                                 FilePreviewComponent* previewComp,
                                                 ComboBox* currentPathBox,
                                                 TextEditor* filenameBox,
                                                 Button* goUpButton)
{
    const auto sectionHeight = 22;
    const auto buttonWidth = 50;

    auto b = browserComp.getLocalBounds().reduced (20, 5);

    auto topSlice    = b.removeFromTop (sectionHeight);
    auto bottomSlice = b.removeFromBottom (sectionHeight);

    currentPathBox->setBounds (topSlice.removeFromLeft (topSlice.getWidth() - buttonWidth));

    topSlice.removeFromLeft (6);
    goUpButton->setBounds (topSlice);

    bottomSlice.removeFromLeft (20);
    filenameBox->setBounds (bottomSlice);

    if (previewComp != nullptr)
        previewComp->setBounds (b.removeFromRight (b.getWidth() / 3));

    if (auto listAsComp = dynamic_cast<Component*> (fileListComponent))
        listAsComp->setBounds (b.reduced (0, 10));
}

void LookAndFeel_V4::drawFileBrowserRow (Graphics& g, int width, int height,
                                         const File& file, const String& filename, Image* icon,
                                         const String& fileSizeDescription,
                                         const String& fileTimeDescription,
                                         bool isDirectory, bool isItemSelected,
                                         int itemIndex, DirectoryContentsDisplayComponent& dcc)
{
    LookAndFeel_V2::drawFileBrowserRow (g, width, height, file, filename, icon,
                                        fileSizeDescription, fileTimeDescription,
                                        isDirectory, isItemSelected, itemIndex, dcc);
}

//==============================================================================
void LookAndFeel_V4::drawPopupMenuItem (Graphics& g, const Rectangle<int>& area,
                                        const bool isSeparator, const bool isActive,
                                        const bool isHighlighted, const bool isTicked,
                                        const bool hasSubMenu, const String& text,
                                        const String& shortcutKeyText,
                                        const Drawable* icon, const Colour* const textColourToUse)
{
    if (isSeparator)
    {
        auto r  = area.reduced (5, 0);
        r.removeFromTop (roundToInt ((r.getHeight() * 0.5f) - 0.5f));

        g.setColour (findColour (PopupMenu::textColourId).withAlpha (0.3f));
        g.fillRect (r.removeFromTop (1));
    }
    else
    {
        auto textColour = (textColourToUse == nullptr ? findColour (PopupMenu::textColourId)
                                                      : *textColourToUse);

        auto r  = area.reduced (1);

        if (isHighlighted && isActive)
        {
            g.setColour (findColour (PopupMenu::highlightedBackgroundColourId));
            g.fillRect (r);

            g.setColour (findColour (PopupMenu::highlightedTextColourId));
        }
        else
        {
            g.setColour (textColour.withMultipliedAlpha (isActive ? 1.0f : 0.5f));
        }

        r.reduce (jmin (5, area.getWidth() / 20), 0);

        auto font = getPopupMenuFont();

        const auto maxFontHeight = r.getHeight() / 1.3f;

        if (font.getHeight() > maxFontHeight)
            font.setHeight (maxFontHeight);

        g.setFont (font);

        auto iconArea = r.removeFromLeft (roundToInt (maxFontHeight)).toFloat();

        if (icon != nullptr)
        {
            icon->drawWithin (g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
            r.removeFromLeft (roundToInt (maxFontHeight * 0.5f));
        }
        else if (isTicked)
        {
            const auto tick = getTickShape (1.0f);
            g.fillPath (tick, tick.getTransformToScaleToFit (iconArea.reduced (iconArea.getWidth() / 5, 0).toFloat(), true));
        }

        if (hasSubMenu)
        {
            const auto arrowH = 0.6f * getPopupMenuFont().getAscent();

            const auto x = (float) r.removeFromRight ((int) arrowH).getX();
            const auto halfH = (float) r.getCentreY();

            Path path;
            path.startNewSubPath (x, halfH - arrowH * 0.5f);
            path.lineTo (x + arrowH * 0.6f, halfH);
            path.lineTo (x, halfH + arrowH * 0.5f);

            g.strokePath (path, PathStrokeType (2.0f));
        }

        r.removeFromRight (3);
        g.drawFittedText (text, r, Justification::centredLeft, 1);

        if (shortcutKeyText.isNotEmpty())
        {
            auto f2 = font;
            f2.setHeight (f2.getHeight() * 0.75f);
            f2.setHorizontalScale (0.95f);
            g.setFont (f2);

            g.drawText (shortcutKeyText, r, Justification::centredRight, true);
        }
    }
}

void LookAndFeel_V4::getIdealPopupMenuItemSize (const String& text, const bool isSeparator,
                                                int standardMenuItemHeight, int& idealWidth, int& idealHeight)
{
    if (isSeparator)
    {
        idealWidth = 50;
        idealHeight = standardMenuItemHeight > 0 ? standardMenuItemHeight / 10 : 10;
    }
    else
    {
        auto font = getPopupMenuFont();

        if (standardMenuItemHeight > 0 && font.getHeight() > standardMenuItemHeight / 1.3f)
            font.setHeight (standardMenuItemHeight / 1.3f);

        idealHeight = standardMenuItemHeight > 0 ? standardMenuItemHeight : roundToInt (font.getHeight() * 1.3f);
        idealWidth = font.getStringWidth (text) + idealHeight * 2;
    }
}

void LookAndFeel_V4::drawMenuBarBackground (Graphics& g, int width, int height,
                                            bool, MenuBarComponent& menuBar)
{
    const auto colour = menuBar.findColour (TextButton::buttonColourId).withAlpha (0.4f);

    Rectangle<int> r (width, height);

    g.setColour (colour.contrasting (0.15f));
    g.fillRect  (r.removeFromTop (1));
    g.fillRect  (r.removeFromBottom (1));

    g.setGradientFill (ColourGradient::vertical (colour, 0, colour.darker (0.2f), (float) height));
    g.fillRect (r);
}

void LookAndFeel_V4::drawMenuBarItem (Graphics& g, int width, int height,
                                      int itemIndex, const String& itemText,
                                      bool isMouseOverItem, bool isMenuOpen,
                                      bool /*isMouseOverBar*/, MenuBarComponent& menuBar)
{
    if (! menuBar.isEnabled())
    {
        g.setColour (menuBar.findColour (TextButton::textColourOffId)
                            .withMultipliedAlpha (0.5f));
    }
    else if (isMenuOpen || isMouseOverItem)
    {
        g.fillAll   (menuBar.findColour (TextButton::buttonOnColourId));
        g.setColour (menuBar.findColour (TextButton::textColourOnId));
    }
    else
    {
        g.setColour (menuBar.findColour (TextButton::textColourOffId));
    }

    g.setFont (getMenuBarFont (menuBar, itemIndex, itemText));
    g.drawFittedText (itemText, 0, 0, width, height, Justification::centred, 1);
}

//==============================================================================
void LookAndFeel_V4::drawComboBox (Graphics& g, int width, int height, bool,
                                   int, int, int, int, ComboBox& box)
{
    const auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    const Rectangle<int> boxBounds (0, 0, width, height);

    g.setColour (box.findColour (ComboBox::backgroundColourId));
    g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

    g.setColour (box.findColour (ComboBox::outlineColourId));
    g.drawRoundedRectangle (boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);

    Rectangle<int> arrowZone (width - 30, 0, 20, height);
    Path path;
    path.startNewSubPath (arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
    path.lineTo (static_cast<float> (arrowZone.getCentreX()), arrowZone.getCentreY() + 3.0f);
    path.lineTo (arrowZone.getRight() - 3.0f, arrowZone.getCentreY() - 2.0f);

    g.setColour (box.findColour (ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
    g.strokePath (path, PathStrokeType (2.0f));
}

Font LookAndFeel_V4::getComboBoxFont (ComboBox& box)
{
    return Font (jmin (16.0f, box.getHeight() * 0.85f));
}

void LookAndFeel_V4::positionComboBoxText (ComboBox& box, Label& label)
{
    label.setBounds (1, 1,
                     box.getWidth() - 30,
                     box.getHeight() - 2);

    label.setFont (getComboBoxFont (box));
}

//==============================================================================
void LookAndFeel_V4::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                       float sliderPos,
                                       float minSliderPos,
                                       float maxSliderPos,
                                       const Slider::SliderStyle style, Slider& slider)
{
    if (slider.isBar())
    {
        g.setColour (slider.findColour (Slider::trackColourId));
        g.fillRect (slider.isHorizontal() ? Rectangle<float> (static_cast<float> (x), y + 0.5f, sliderPos - x, height - 1.0f)
                                          : Rectangle<float> (x + 0.5f, sliderPos, width - 1.0f, y + (height - sliderPos)));
    }
    else
    {
        const auto isTwoVal   = (style == Slider::SliderStyle::TwoValueVertical   || style == Slider::SliderStyle::TwoValueHorizontal);
        const auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

        const auto trackWidth = jmin (6.0f, slider.isHorizontal() ? height * 0.25f : width * 0.25f);

        const Point<float> startPoint (slider.isHorizontal() ? x : x + width * 0.5f,
                                       slider.isHorizontal() ? y + height * 0.5f : height + y);

        const Point<float> endPoint (slider.isHorizontal() ? width + x : startPoint.x,
                                     slider.isHorizontal() ? startPoint.y : y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, PathStrokeType (trackWidth, PathStrokeType::curved, PathStrokeType::rounded));

        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        if (isTwoVal || isThreeVal)
        {
            minPoint = { slider.isHorizontal() ? minSliderPos : width * 0.5f,
                         slider.isHorizontal() ? height * 0.5f : minSliderPos };

            if (isThreeVal)
                thumbPoint = { slider.isHorizontal() ? sliderPos : width * 0.5f,
                               slider.isHorizontal() ? height * 0.5f : sliderPos };

            maxPoint = { slider.isHorizontal() ? maxSliderPos : width * 0.5f,
                         slider.isHorizontal() ? height * 0.5f : maxSliderPos };
        }
        else
        {
            const auto kx = slider.isHorizontal() ? sliderPos : (x + width * 0.5f);
            const auto ky = slider.isHorizontal() ? (y + height * 0.5f) : sliderPos;

            minPoint = startPoint;
            maxPoint = { kx, ky };
        }

        const auto thumbWidth = trackWidth * 2.0f;

        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (isThreeVal ? thumbPoint : maxPoint);
        g.setColour (slider.findColour (Slider::trackColourId));
        g.strokePath (valueTrack, PathStrokeType (trackWidth, PathStrokeType::curved, PathStrokeType::rounded));

        if (! isTwoVal)
        {
            g.setColour (slider.findColour (Slider::thumbColourId));
            g.fillEllipse (Rectangle<float> (thumbWidth, thumbWidth).withCentre (isThreeVal ? thumbPoint : maxPoint));
        }

        if (isTwoVal || isThreeVal)
        {
            const auto sr = jmin (trackWidth, (slider.isHorizontal() ? height : width) * 0.4f);
            const auto pointerColour = slider.findColour (Slider::thumbColourId);

            if (slider.isHorizontal())
            {
                drawPointer (g, minSliderPos - sr,
                             jmax (0.0f, y + height * 0.5f - trackWidth * 2.0f),
                             trackWidth * 2.0f, pointerColour, 2);

                drawPointer (g, maxSliderPos - trackWidth,
                             jmin (y + height - trackWidth * 2.0f, y + height * 0.5f),
                             trackWidth * 2.0f, pointerColour, 4);
            }
            else
            {
                drawPointer (g, jmax (0.0f, x + width * 0.5f - trackWidth * 2.0f),
                             minSliderPos - trackWidth,
                             trackWidth * 2.0f, pointerColour, 1);

                drawPointer (g, jmin (x + width - trackWidth * 2.0f, x + width * 0.5f), maxSliderPos - sr,
                             trackWidth * 2.0f, pointerColour, 3);
            }
        }
    }
}

void LookAndFeel_V4::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                                       const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    const auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);
    const auto fill    = slider.findColour (Slider::rotarySliderFillColourId);

    const auto bounds = Rectangle<int> (x, y, width, height).toFloat().reduced (10);

    auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    const auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin (8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc (bounds.getCentreX(),
                                 bounds.getCentreY(),
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                toAngle,
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));
    }

    const auto thumbWidth = lineW * 2.0f;
    const Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - float_Pi * 0.5f),
                                   bounds.getCentreY() + arcRadius * std::sin (toAngle - float_Pi * 0.5f));

    g.setColour (slider.findColour (Slider::thumbColourId));
    g.fillEllipse (Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
}

void LookAndFeel_V4::drawPointer (Graphics& g, const float x, const float y, const float diameter,
                                  const Colour& colour, const int direction) noexcept
{
    Path p;
    p.startNewSubPath (x + diameter * 0.5f, y);
    p.lineTo (x + diameter, y + diameter * 0.6f);
    p.lineTo (x + diameter, y + diameter);
    p.lineTo (x, y + diameter);
    p.lineTo (x, y + diameter * 0.6f);
    p.closeSubPath();

    p.applyTransform (AffineTransform::rotation (direction * (float_Pi * 0.5f), x + diameter * 0.5f, y + diameter * 0.5f));

    g.setColour (colour);
    g.fillPath (p);
}

//==============================================================================
void LookAndFeel_V4::drawTooltip (Graphics& g, const String& text, int width, int height)
{
    Rectangle<int> bounds (width, height);
    const auto cornerSize = 5.0f;

    g.setColour (findColour (TooltipWindow::backgroundColourId));
    g.fillRoundedRectangle (bounds.toFloat(), cornerSize);

    g.setColour (findColour (TooltipWindow::outlineColourId));
    g.drawRoundedRectangle (bounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);

    LookAndFeelHelpers::layoutTooltipText (text, findColour (TooltipWindow::textColourId))
                                         .draw (g, Rectangle<float> ((float) width, (float) height));
}

//==============================================================================
void LookAndFeel_V4::drawConcertinaPanelHeader (Graphics& g, const Rectangle<int>& area,
                                                bool isMouseOver, bool /*isMouseDown*/,
                                                ConcertinaPanel& concertina, Component& panel)
{
    auto bounds = area.toFloat().reduced (0.5f);
    const auto cornerSize = 4.0f;
    auto isTopPanel = (concertina.getPanel (0) == &panel);

    Path p;
    p.addRoundedRectangle (bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                           cornerSize, cornerSize, isTopPanel, isTopPanel, false, false);

    const auto bkg = Colours::grey;

    g.setGradientFill (ColourGradient::vertical (Colours::white.withAlpha (isMouseOver ? 0.4f : 0.2f), (float) area.getY(),
                                                 Colours::darkgrey.withAlpha (0.1f), (float) area.getBottom()));
    g.fillPath (p);
}

//==============================================================================
void LookAndFeel_V4::drawLevelMeter (Graphics& g, int width, int height, float level)
{
    const auto outerCornerSize = 3.0f;
    const auto outerBorderWidth = 2.0f;
    const auto totalBlocks = 7;
    const auto spacingFraction = 0.03f;

    g.setColour (findColour (ResizableWindow::backgroundColourId));
    g.fillRoundedRectangle (0.0f, 0.0f, (float) width, (float) height, outerCornerSize);

    const auto doubleOuterBorderWidth = 2.0f * outerBorderWidth;
    const auto numBlocks = roundToInt (totalBlocks * level);

    const auto blockWidth = (width - doubleOuterBorderWidth) / (float) totalBlocks;
    const auto blockHeight = height - doubleOuterBorderWidth;

    const auto blockRectWidth = (1.0f - 2.0f * spacingFraction) * blockWidth;
    const auto blockRectSpacing = spacingFraction * blockWidth;

    const auto blockCornerSize = 0.1f * blockWidth;

    const auto c = findColour (Slider::thumbColourId);

    for (int i = 0; i < totalBlocks; ++i)
    {
        if (i >= numBlocks)
            g.setColour (c.withAlpha (0.5f));
        else
            g.setColour (i < totalBlocks - 1 ? c : Colours::red);

        g.fillRoundedRectangle (outerBorderWidth + (i * blockWidth) + blockRectSpacing,
                                outerBorderWidth,
                                blockRectWidth,
                                blockHeight,
                                blockCornerSize);
    }
}

//==============================================================================
void LookAndFeel_V4::paintToolbarBackground (Graphics& g, int w, int h, Toolbar& toolbar)
{
    const auto background = toolbar.findColour (Toolbar::backgroundColourId);

    g.setGradientFill (ColourGradient (background, 0.0f, 0.0f,
                                       background.darker (0.2f),
                                       toolbar.isVertical() ? w - 1.0f : 0.0f,
                                       toolbar.isVertical() ? 0.0f : h - 1.0f,
                                       false));
    g.fillAll();
}

void LookAndFeel_V4::paintToolbarButtonLabel (Graphics& g, int x, int y, int width, int height,
                                              const String& text, ToolbarItemComponent& component)
{
    auto baseTextColour = component.findParentComponentOfClass<PopupMenu::CustomComponent>() != nullptr
                              ? component.findColour (PopupMenu::textColourId)
                              : component.findColour (Toolbar::labelTextColourId);

    g.setColour (baseTextColour.withAlpha (component.isEnabled() ? 1.0f : 0.25f));

    const auto fontHeight = jmin (14.0f, height * 0.85f);
    g.setFont (fontHeight);

    g.drawFittedText (text,
                      x, y, width, height,
                      Justification::centred,
                      jmax (1, height / (int) fontHeight));
}

//==============================================================================
void LookAndFeel_V4::drawPropertyPanelSectionHeader (Graphics& g, const String& name,
                                                     bool isOpen, int width, int height)
{
    const auto buttonSize = height * 0.75f;
    const auto buttonIndent = (height - buttonSize) * 0.5f;

    drawTreeviewPlusMinusBox (g, Rectangle<float> (buttonIndent, buttonIndent, buttonSize, buttonSize),
                              findColour (ResizableWindow::backgroundColourId), isOpen, false);

    const auto textX = (int) (buttonIndent * 2.0f + buttonSize + 2.0f);

    g.setColour (findColour (PropertyComponent::labelTextColourId));

    g.setFont (Font (height * 0.7f, Font::bold));
    g.drawText (name, textX, 0, width - textX - 4, height, Justification::centredLeft, true);
}

void LookAndFeel_V4::drawPropertyComponentBackground (Graphics& g, int width, int height, PropertyComponent& component)
{
    g.setColour (component.findColour (PropertyComponent::backgroundColourId));
    g.fillRect  (0, 0, width, height - 1);
}

void LookAndFeel_V4::drawPropertyComponentLabel (Graphics& g, int width, int height, PropertyComponent& component)
{
    ignoreUnused (width);

    const auto indent = getPropertyComponentIndent (component);

    g.setColour (component.findColour (PropertyComponent::labelTextColourId)
                          .withMultipliedAlpha (component.isEnabled() ? 1.0f : 0.6f));

    g.setFont (jmin (height, 24) * 0.65f);

    auto r = getPropertyComponentContentPosition (component);

    g.drawFittedText (component.getName(),
                      indent, r.getY(), r.getX() - 5, r.getHeight(),
                      Justification::centredLeft, 2);
}

int LookAndFeel_V4::getPropertyComponentIndent (PropertyComponent& component)
{
    return jmin (10, component.getWidth() / 10);
}

Rectangle<int> LookAndFeel_V4::getPropertyComponentContentPosition (PropertyComponent& component)
{
    const auto textW = jmin (200, component.getWidth() / 2);
    return { textW, 0, component.getWidth() - textW, component.getHeight() - 1 };
}

//==============================================================================
void LookAndFeel_V4::drawCallOutBoxBackground (CallOutBox& box, Graphics& g,
                                               const Path& path, Image& cachedImage)
{
    if (cachedImage.isNull())
    {
        cachedImage = Image (Image::ARGB, box.getWidth(), box.getHeight(), true);
        Graphics g2 (cachedImage);

        DropShadow (Colours::black.withAlpha (0.7f), 8, Point<int> (0, 2)).drawForPath (g2, path);
    }

    g.setColour (Colours::black);
    g.drawImageAt (cachedImage, 0, 0);

    g.setColour (currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).withAlpha (0.8f));
    g.fillPath (path);

    g.setColour (currentColourScheme.getUIColour (ColourScheme::UIColour::outline).withAlpha (0.8f));
    g.strokePath (path, PathStrokeType (2.0f));
}

//==============================================================================
void LookAndFeel_V4::drawStretchableLayoutResizerBar (Graphics& g, int /*w*/, int /*h*/, bool /*isVerticalBar*/,
                                      bool isMouseOver, bool isMouseDragging)
{
    if (isMouseOver || isMouseDragging)
        g.fillAll (currentColourScheme.getUIColour (ColourScheme::UIColour::defaultFill).withAlpha (0.5f));
}

//==============================================================================
void LookAndFeel_V4::initialiseColours()
{
    const uint32 transparent = 0x00000000;

    const uint32 coloursToUse[] =
    {
        TextButton::buttonColourId,                 currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        TextButton::buttonOnColourId,               currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedFill).getARGB(),
        TextButton::textColourOnId,                 currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedText).getARGB(),
        TextButton::textColourOffId,                currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),

        ToggleButton::textColourId,                 currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),
        ToggleButton::tickColourId,                 currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),
        ToggleButton::tickDisabledColourId,         currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).withAlpha (0.5f).getARGB(),

        TextEditor::backgroundColourId,             currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        TextEditor::textColourId,                   currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),
        TextEditor::highlightColourId,              currentColourScheme.getUIColour (ColourScheme::UIColour::defaultFill).withAlpha (0.4f).getARGB(),
        TextEditor::highlightedTextColourId,        currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedText).getARGB(),
        TextEditor::outlineColourId,                currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),
        TextEditor::focusedOutlineColourId,         currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),
        TextEditor::shadowColourId,                 transparent,

        CaretComponent::caretColourId,              currentColourScheme.getUIColour (ColourScheme::UIColour::defaultFill).getARGB(),

        Label::backgroundColourId,                  transparent,
        Label::textColourId,                        currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),
        Label::outlineColourId,                     transparent,
        Label::textWhenEditingColourId,             currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),

        ScrollBar::backgroundColourId,              transparent,
        ScrollBar::thumbColourId,                   currentColourScheme.getUIColour (ColourScheme::UIColour::defaultFill).getARGB(),
        ScrollBar::trackColourId,                   transparent,

        TreeView::linesColourId,                    transparent,
        TreeView::backgroundColourId,               transparent,
        TreeView::dragAndDropIndicatorColourId,     currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),
        TreeView::selectedItemBackgroundColourId,   transparent,
        TreeView::oddItemsColourId,                 transparent,
        TreeView::evenItemsColourId,                transparent,

        PopupMenu::backgroundColourId,              currentColourScheme.getUIColour (ColourScheme::UIColour::menuBackground).getARGB(),
        PopupMenu::textColourId,                    currentColourScheme.getUIColour (ColourScheme::UIColour::menuText).getARGB(),
        PopupMenu::headerTextColourId,              currentColourScheme.getUIColour (ColourScheme::UIColour::menuText).getARGB(),
        PopupMenu::highlightedTextColourId,         currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedText).getARGB(),
        PopupMenu::highlightedBackgroundColourId,   currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedFill).getARGB(),

        ComboBox::buttonColourId,                   currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),
        ComboBox::outlineColourId,                  currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),
        ComboBox::textColourId,                     currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),
        ComboBox::backgroundColourId,               currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        ComboBox::arrowColourId,                    currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),
        ComboBox::focusedOutlineColourId,           currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),

        PropertyComponent::backgroundColourId,      currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        PropertyComponent::labelTextColourId,       currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),

        TextPropertyComponent::backgroundColourId,  currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        TextPropertyComponent::textColourId,        currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),
        TextPropertyComponent::outlineColourId,     currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),

        BooleanPropertyComponent::backgroundColourId, currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        BooleanPropertyComponent::outlineColourId,    currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),

        ListBox::backgroundColourId,                currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        ListBox::outlineColourId,                   currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),
        ListBox::textColourId,                      currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),

        Slider::backgroundColourId,                 currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        Slider::thumbColourId,                      currentColourScheme.getUIColour (ColourScheme::UIColour::defaultFill).getARGB(),
        Slider::trackColourId,                      currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedFill).getARGB(),
        Slider::rotarySliderFillColourId,           currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedFill).getARGB(),
        Slider::rotarySliderOutlineColourId,        currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        Slider::textBoxTextColourId,                currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),
        Slider::textBoxBackgroundColourId,          currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).withAlpha (0.0f).getARGB(),
        Slider::textBoxHighlightColourId,           currentColourScheme.getUIColour (ColourScheme::UIColour::defaultFill).withAlpha (0.4f).getARGB(),
        Slider::textBoxOutlineColourId,             currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),

        ResizableWindow::backgroundColourId,        currentColourScheme.getUIColour (ColourScheme::UIColour::windowBackground).getARGB(),

        DocumentWindow::textColourId,               currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),

        AlertWindow::backgroundColourId,            currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        AlertWindow::textColourId,                  currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),
        AlertWindow::outlineColourId,               currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),

        ProgressBar::backgroundColourId,            currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        ProgressBar::foregroundColourId,            currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedFill).getARGB(),

        TooltipWindow::backgroundColourId,          currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedFill).getARGB(),
        TooltipWindow::textColourId,                currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedText).getARGB(),
        TooltipWindow::outlineColourId,             transparent,

        TabbedComponent::backgroundColourId,        transparent,
        TabbedComponent::outlineColourId,           currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),
        TabbedButtonBar::tabOutlineColourId,        currentColourScheme.getUIColour (ColourScheme::UIColour::outline).withAlpha (0.5f).getARGB(),
        TabbedButtonBar::frontOutlineColourId,      currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),

        Toolbar::backgroundColourId,                currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).withAlpha (0.4f).getARGB(),
        Toolbar::separatorColourId,                 currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),
        Toolbar::buttonMouseOverBackgroundColourId, currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).contrasting (0.2f).getARGB(),
        Toolbar::buttonMouseDownBackgroundColourId, currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).contrasting (0.5f).getARGB(),
        Toolbar::labelTextColourId,                 currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),
        Toolbar::editingModeOutlineColourId,        currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),

        DrawableButton::textColourId,               currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),
        DrawableButton::textColourOnId,             currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedText).getARGB(),
        DrawableButton::backgroundColourId,         transparent,
        DrawableButton::backgroundOnColourId,       currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedFill).getARGB(),

        HyperlinkButton::textColourId,              currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).interpolatedWith (Colours::blue, 0.4f).getARGB(),

        GroupComponent::outlineColourId,            currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),
        GroupComponent::textColourId,               currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),

        BubbleComponent::backgroundColourId,        currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        BubbleComponent::outlineColourId,           currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),

        DirectoryContentsDisplayComponent::highlightColourId,          currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedFill).getARGB(),
        DirectoryContentsDisplayComponent::textColourId,               currentColourScheme.getUIColour (ColourScheme::UIColour::menuText).getARGB(),
        DirectoryContentsDisplayComponent::highlightedTextColourId,    currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedText).getARGB(),

        0x1000440, /*LassoComponent::lassoFillColourId*/        currentColourScheme.getUIColour (ColourScheme::UIColour::defaultFill).getARGB(),
        0x1000441, /*LassoComponent::lassoOutlineColourId*/     currentColourScheme.getUIColour (ColourScheme::UIColour::outline).getARGB(),

        0x1005000, /*MidiKeyboardComponent::whiteNoteColourId*/               0xffffffff,
        0x1005001, /*MidiKeyboardComponent::blackNoteColourId*/               0xff000000,
        0x1005002, /*MidiKeyboardComponent::keySeparatorLineColourId*/        0x66000000,
        0x1005003, /*MidiKeyboardComponent::mouseOverKeyOverlayColourId*/     0x80ffff00,
        0x1005004, /*MidiKeyboardComponent::keyDownOverlayColourId*/          0xffb6b600,
        0x1005005, /*MidiKeyboardComponent::textLabelColourId*/               0xff000000,
        0x1005006, /*MidiKeyboardComponent::upDownButtonBackgroundColourId*/  0xffd3d3d3,
        0x1005007, /*MidiKeyboardComponent::upDownButtonArrowColourId*/       0xff000000,
        0x1005008, /*MidiKeyboardComponent::shadowColourId*/                  0x4c000000,

        0x1004500, /*CodeEditorComponent::backgroundColourId*/                currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        0x1004502, /*CodeEditorComponent::highlightColourId*/                 currentColourScheme.getUIColour (ColourScheme::UIColour::defaultFill).withAlpha (0.4f).getARGB(),
        0x1004503, /*CodeEditorComponent::defaultTextColourId*/               currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),
        0x1004504, /*CodeEditorComponent::lineNumberBackgroundId*/            currentColourScheme.getUIColour (ColourScheme::UIColour::highlightedFill).withAlpha (0.5f).getARGB(),
        0x1004505, /*CodeEditorComponent::lineNumberTextId*/                  currentColourScheme.getUIColour (ColourScheme::UIColour::defaultFill).getARGB(),

        0x1007000, /*ColourSelector::backgroundColourId*/                     currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        0x1007001, /*ColourSelector::labelTextColourId*/                      currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),

        0x100ad00, /*KeyMappingEditorComponent::backgroundColourId*/          currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        0x100ad01, /*KeyMappingEditorComponent::textColourId*/                currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),

        FileSearchPathListComponent::backgroundColourId,        currentColourScheme.getUIColour (ColourScheme::UIColour::menuBackground).getARGB(),

        FileChooserDialogBox::titleTextColourId,                currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),

        SidePanel::backgroundColour,                            currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).getARGB(),
        SidePanel::titleTextColour,                             currentColourScheme.getUIColour (ColourScheme::UIColour::defaultText).getARGB(),
        SidePanel::shadowBaseColour,                            currentColourScheme.getUIColour (ColourScheme::UIColour::widgetBackground).darker().getARGB(),
        SidePanel::dismissButtonNormalColour,                   currentColourScheme.getUIColour (ColourScheme::UIColour::defaultFill).getARGB(),
        SidePanel::dismissButtonOverColour,                     currentColourScheme.getUIColour (ColourScheme::UIColour::defaultFill).darker().getARGB(),
        SidePanel::dismissButtonDownColour,                     currentColourScheme.getUIColour (ColourScheme::UIColour::defaultFill).brighter().getARGB(),

        FileBrowserComponent::currentPathBoxBackgroundColourId,    currentColourScheme.getUIColour (ColourScheme::UIColour::menuBackground).getARGB(),
        FileBrowserComponent::currentPathBoxTextColourId,          currentColourScheme.getUIColour (ColourScheme::UIColour::menuText).getARGB(),
        FileBrowserComponent::currentPathBoxArrowColourId,         currentColourScheme.getUIColour (ColourScheme::UIColour::menuText).getARGB(),
        FileBrowserComponent::filenameBoxBackgroundColourId,       currentColourScheme.getUIColour (ColourScheme::UIColour::menuBackground).getARGB(),
        FileBrowserComponent::filenameBoxTextColourId,             currentColourScheme.getUIColour (ColourScheme::UIColour::menuText).getARGB(),
    };

    for (int i = 0; i < numElementsInArray (coloursToUse); i += 2)
        setColour ((int) coloursToUse [i], Colour ((uint32) coloursToUse [i + 1]));
}

} // namespace juce
