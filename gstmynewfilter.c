/**
 * SECTION:element-mynewfilter
 *
 * FIXME:Describe mynewfilter here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! mynewfilter ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gstmynewfilter.h"

GST_DEBUG_CATEGORY_STATIC (gst_mynewfilter_debug);
#define GST_CAT_DEFAULT gst_mynewfilter_debug

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

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
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

#define gst_mynewfilter_parent_class parent_class
G_DEFINE_TYPE (Gstmynewfilter, gst_mynewfilter, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE (mynewfilter, "mynewfilter", GST_RANK_NONE,
    GST_TYPE_MYNEWFILTER);

static void gst_mynewfilter_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_mynewfilter_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_mynewfilter_sink_event (GstPad * pad,
    GstObject * parent, GstEvent * event);
static GstFlowReturn gst_mynewfilter_chain (GstPad * pad,
    GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the mynewfilter's class */
static void
gst_mynewfilter_class_init (GstmynewfilterClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_mynewfilter_set_property;
  gobject_class->get_property = gst_mynewfilter_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE));

  gst_element_class_set_details_simple (gstelement_class,
      "mynewfilter",
      "FIXME:Generic",
      "FIXME:Generic Template Element", "Meera <<user@hostname.org>>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_mynewfilter_init (Gstmynewfilter * filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_event_function (filter->sinkpad,
      GST_DEBUG_FUNCPTR (gst_mynewfilter_sink_event));
  gst_pad_set_chain_function (filter->sinkpad,
      GST_DEBUG_FUNCPTR (gst_mynewfilter_chain));
  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  filter->silent = FALSE;
}

static void
gst_mynewfilter_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  Gstmynewfilter *filter = GST_MYNEWFILTER (object);

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
gst_mynewfilter_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  Gstmynewfilter *filter = GST_MYNEWFILTER (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_mynewfilter_sink_event (GstPad * pad, GstObject * parent,
    GstEvent * event)
{
  Gstmynewfilter *filter;
  gboolean ret;

  filter = GST_MYNEWFILTER (parent);

// Logging events //
  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps *caps;
      gst_event_parse_caps (event, &caps);
      /* do something with the caps */
      /* and forward */
      //g_print(caps);
      ret = gst_pad_event_default (pad, parent, event);
      break;
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_mynewfilter_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  Gstmynewfilter *filter;
  GstMapInfo map;
  gfloat *raw;
  long bufsize, num_frames, i;
  bufsize = gst_buffer_get_size(buf);
  num_frames = bufsize / 4; // Number of audio frames == buffer size / size of one sample
  gst_buffer_map(buf, &map, GST_MAP_WRITE);
  raw = (gfloat*)map.data;
  filter = GST_MYNEWFILTER (parent);
  if (filter->silent == FALSE){
    for(i = 0; i < num_frames; i++) {
        raw[i] = raw[i] / 3;
    }
    gst_buffer_unmap (buf, &map);
  }
  return gst_pad_push (filter->srcpad, buf); //
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
mynewfilter_init (GstPlugin * mynewfilter)
{
  /* debug category for filtering log messages
   *
   * exchange the string 'Template mynewfilter' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_mynewfilter_debug, "mynewfilter",
      0, "Template mynewfilter");

  return GST_ELEMENT_REGISTER (mynewfilter, mynewfilter);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstmynewfilter"
#endif

/* gstreamer looks for this structure to register mynewfilters
 *
 * exchange the string 'Template mynewfilter' with your mynewfilter description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    mynewfilter,
    "mynewfilter",
    mynewfilter_init,
    PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
