

#include <errno.h>
#include <unistd.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <webkit/webkit.h>

#if GLIB_CHECK_VERSION(2, 16, 0) && GTK_CHECK_VERSION(2, 14, 0)

static void test_network_request_create_destroy()
{
    WebKitNetworkRequest* request;
    SoupMessage* message;

    /* Test creation with URI */
    request = WEBKIT_NETWORK_REQUEST(g_object_new(WEBKIT_TYPE_NETWORK_REQUEST, "uri", "http://debian.org/", NULL));
    g_assert(WEBKIT_IS_NETWORK_REQUEST(request));
    message = webkit_network_request_get_message(request);
    g_assert(!message);
    g_object_unref(request);

    /* Test creation with SoupMessage */
    message = soup_message_new("GET", "http://debian.org/");
    request = WEBKIT_NETWORK_REQUEST(g_object_new(WEBKIT_TYPE_NETWORK_REQUEST, "message", message, NULL));
    g_assert(WEBKIT_IS_NETWORK_REQUEST(request));
    g_assert_cmpint(G_OBJECT(message)->ref_count, ==, 2);
    g_object_unref(request);
    g_assert_cmpint(G_OBJECT(message)->ref_count, ==, 1);
    g_object_unref(message);

    /* Test creation with both SoupMessage and URI */
    message = soup_message_new("GET", "http://debian.org/");
    request = WEBKIT_NETWORK_REQUEST(g_object_new(WEBKIT_TYPE_NETWORK_REQUEST, "message", message, "uri", "http://gnome.org/", NULL));
    g_assert(WEBKIT_IS_NETWORK_REQUEST(request));
    g_assert_cmpint(G_OBJECT(message)->ref_count, ==, 2);
    g_assert_cmpstr(webkit_network_request_get_uri(request), ==, "http://gnome.org/");
    g_object_unref(request);
    g_assert_cmpint(G_OBJECT(message)->ref_count, ==, 1);
    g_object_unref(message);
}

static void test_network_request_properties()
{
    WebKitNetworkRequest* request;
    SoupMessage* message;
    gchar* soupURI;

    /* Test URI is set correctly when creating with URI */
    request = webkit_network_request_new("http://debian.org/");
    g_assert(WEBKIT_IS_NETWORK_REQUEST(request));
    g_assert_cmpstr(webkit_network_request_get_uri(request), ==, "http://debian.org/");
    g_object_unref(request);

    /* Test URI is set correctly when creating with Message */
    message = soup_message_new("GET", "http://debian.org/");
    request = WEBKIT_NETWORK_REQUEST(g_object_new(WEBKIT_TYPE_NETWORK_REQUEST, "message", message, NULL));
    g_assert(WEBKIT_IS_NETWORK_REQUEST(request));
    g_object_unref(message);

    message = webkit_network_request_get_message(request);
    soupURI = soup_uri_to_string(soup_message_get_uri(message), FALSE);
    g_assert_cmpstr(soupURI, ==, "http://debian.org/");
    g_free(soupURI);

    g_assert_cmpstr(webkit_network_request_get_uri(request), ==, "http://debian.org/");
    g_object_unref(request);
}

int main(int argc, char** argv)
{
    g_thread_init(NULL);
    gtk_test_init(&argc, &argv, NULL);

    g_test_bug_base("https://bugs.webkit.org/");
    g_test_add_func("/webkit/networkrequest/createdestroy", test_network_request_create_destroy);
    g_test_add_func("/webkit/networkrequest/properties", test_network_request_properties);
    return g_test_run ();
}

#else
int main(int argc, char** argv)
{
    g_critical("You will need at least glib-2.16.0 and gtk-2.14.0 to run the unit tests. Doing nothing now.");
    return 0;
}

#endif
