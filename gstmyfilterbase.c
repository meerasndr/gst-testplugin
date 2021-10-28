#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include "gstmyfilter.h"

GST_DEBUG_CATEGORY_STATIC (gst_myfilter_debug);
#define GST_CAT_DEFAULT gst_myfilter_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT
};

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw," "format = (string)F32LE")
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw," "format = (string)F32LE")
  );

GST_ELEMENT_REGISTER_DEFINE (myfilter, "myfilter", GST_RANK_NONE,
    GST_TYPE_MYFILTER);
/*prototypes*/
static void gst_myfilter_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_myfilter_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);
static GstFlowReturn gst_myfilter_transform_ip (GstBaseTransform * trans, GstBuffer * buf);

G_DEFINE_TYPE_WITH_CODE(Gstmyfilter, gst_myfilter, GST_TYPE_BASE_TRANSFORM,
  GST_DEBUG_CATEGORY_INIT (gst_myfilter_debug, "myfilter", 0,
    "debug category for myfilter element"));
static void
gst_myfilter_class_init (GstmyfilterClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseTransformClass *base_transform_class =
    GST_BASE_TRANSFORM_CLASS (klass);

  gst_element_class_add_pad_template (GST_ELEMENT_CLASS(base_transform_class),
    gst_static_pad_template_get(&src_factory));
  gst_element_class_add_pad_template (GST_ELEMENT_CLASS(base_transform_class),
    gst_static_pad_template_get(&sink_factory));

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE));
  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS (klass),
    "FIXME Long name", "Generic", "FIXME Description",
    "FIXME <fixme@example.com>");

  gobject_class->set_property = gst_myfilter_set_property;
  gobject_class->get_property = gst_myfilter_get_property;
  base_transform_class->transform_ip = GST_DEBUG_FUNCPTR (gst_myfilter_transform_ip);
}

static void
gst_myfilter_init (Gstmyfilter * myfilter)
{
  myfilter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  GST_PAD_SET_PROXY_CAPS (myfilter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (myfilter), myfilter->sinkpad);

  myfilter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  GST_PAD_SET_PROXY_CAPS (myfilter->srcpad);
  gst_element_add_pad (GST_ELEMENT (myfilter), myfilter->srcpad);

  myfilter->silent = FALSE;
}

static void
gst_myfilter_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  Gstmyfilter *filter = GST_MYFILTER (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_myfilter_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  Gstmyfilter *filter = GST_MYFILTER (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

// virtual functions

static GstFlowReturn
gst_myfilter_transform_ip (GstBaseTransform * trans, GstBuffer * buf)
{
  Gstmyfilter *filter = GST_MYFILTER (trans);
  GST_DEBUG_OBJECT (filter, "transform_ip");
  GstMapInfo map;
  gfloat *raw;
  long bufsize, num_frames, i;
  bufsize = gst_buffer_get_size(buf);
  num_frames = bufsize / 4; // Number of audio frames == buffer size / size of one sample
  gst_buffer_map(buf, &map, GST_MAP_WRITE);
  raw = (gfloat*)map.data;

  if (filter->silent == FALSE){
    for(i = 0; i < num_frames; i++) {
        raw[i] = raw[i] / 3;
    }
    gst_buffer_unmap (buf, &map);
  }

  return GST_FLOW_OK;
}


// End of Virtual functions

static gboolean
plugin_init (GstPlugin * plugin)
{

  /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
  return gst_element_register (plugin, "myfilter", GST_RANK_NONE,
      GST_TYPE_MYFILTER);
}

/* FIXME: these are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION "0.0.FIXME"
#endif
#ifndef PACKAGE
#define PACKAGE "FIXME_package"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "FIXME_package_name"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "http://FIXME.org/"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    myfilter,
    "FIXME plugin description",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)
