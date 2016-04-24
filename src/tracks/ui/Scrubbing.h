/**********************************************************************

Audacity: A Digital Audio Editor

Scrubbing.h

Paul Licameli split from TrackPanel.cpp

**********************************************************************/

#ifndef __AUDACITY_SCRUBBING__
#define __AUDACITY_SCRUBBING__

#include <vector>
#include <wx/event.h>
#include <wx/longlong.h>

#include "../../Experimental.h"
#include "../../TrackPanelOverlay.h"

class AudacityProject;

// Scrub state object
class Scrubber : public wxEvtHandler
{
public:
   Scrubber(AudacityProject *project);
   ~Scrubber();

   void MarkScrubStart(
      wxCoord xx
#ifdef EXPERIMENTAL_SCRUBBING_SMOOTH_SCROLL
      , bool smoothScrolling
#endif
      , bool alwaysSeeking // if false, can switch seeking or scrubbing
                           // by mouse button state
   );
   // Returns true iff the event should be considered consumed by this:
   bool MaybeStartScrubbing(const wxMouseEvent &event);
   void ContinueScrubbing();

   // This is meant to be called only from ControlToolBar
   void StopScrubbing();

   wxCoord GetScrubStartPosition() const
   { return mScrubStartPosition; }

   // True iff the user has clicked to start scrub and not yet stopped,
   // but IsScrubbing() may yet be false
   bool HasStartedScrubbing() const
   { return GetScrubStartPosition() >= 0; }
   bool IsScrubbing() const;
   bool IsScrollScrubbing() const // If true, implies HasStartedScrubbing()
   { return mSmoothScrollingScrub; }
   bool IsAlwaysSeeking() const
   { return mAlwaysSeeking; }

   bool ShouldDrawScrubSpeed();
   double FindScrubSpeed(bool seeking, double time) const;
   double GetMaxScrubSpeed() const { return mMaxScrubSpeed; }

   void HandleScrollWheel(int steps);

   void SetSeeking() { mScrubSeekPress = true; }
   bool PollIsSeeking();

   // This returns the same as the enabled state of the menu items:
   bool CanScrub() const;
   void AddMenuItems();

   void OnScrub();
   void OnScrollScrub();
   void OnSeek();
   void OnScrollSeek();

   // A string to put in the leftmost part of the status bar.
   const wxString &GetUntranslatedStateString() const;

   // All possible status strings.
   static std::vector<wxString> GetAllUntranslatedStatusStrings();

private:
   void DoScrub(bool scroll, bool seek);
   void OnActivateOrDeactivateApp(wxActivateEvent & event);
   void UncheckAllMenuItems();
   void CheckMenuItem();

private:
   int mScrubToken;
   wxLongLong mScrubStartClockTimeMillis;
   bool mScrubHasFocus;
   int mScrubSpeedDisplayCountdown;
   wxCoord mScrubStartPosition;
   double mMaxScrubSpeed;
   bool mScrubSeekPress;
   bool mSmoothScrollingScrub;
   bool mAlwaysSeeking{};

#ifdef EXPERIMENTAL_SCRUBBING_SCROLL_WHEEL
   int mLogMaxScrubSpeed;
#endif

   AudacityProject *mProject;
};

// Specialist in drawing the scrub speed, and listening for certain events
class ScrubbingOverlay final : public wxEvtHandler, public TrackPanelOverlay
{
public:
   ScrubbingOverlay(AudacityProject *project);
   virtual ~ScrubbingOverlay();

private:
   std::pair<wxRect, bool> DoGetRectangle(wxSize size) override;
   void Draw
      (wxDC &dc, TrackPanelCellIterator begin, TrackPanelCellIterator end) override;

   void OnTimer(wxCommandEvent &event);

   const Scrubber &GetScrubber() const;
   Scrubber &GetScrubber();

   AudacityProject *mProject;

   wxRect mLastScrubRect, mNextScrubRect;
   wxString mLastScrubSpeedText, mNextScrubSpeedText;
};

#endif
