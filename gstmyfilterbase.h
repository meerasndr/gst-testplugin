// We want to rename this as gstmyfilter.h during execution

#ifndef __GST_MYFILTER_H__
#define __GST_MYFILTER_H__

#include <gst/base/gstbasetransform.h>

G_BEGIN_DECLS

#define GST_TYPE_MYFILTER (gst_myfilter_get_type())
G_DECLARE_FINAL_TYPE (Gstmyfilter, gst_myfilter,
    GST, MYFILTER, GstBaseTransform)
struct _Gstmyfilter
{
  GstBaseTransform basetransform;
  GstPad *sinkpad, *srcpad;
  gboolean silent;
};


G_END_DECLS

#endif
