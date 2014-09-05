

#include <glm/gtc/random.hpp>
#include <glm/gtc/noise.hpp>

#include <array>
#include <algorithm>
#include <random>

#include <glbinding/gl/gl.h>
#include <glbinding/gl/extension.h>

#include <globjects/logging.h>
#include <globjects/State.h>
#include <globjects/Capability.h>
#include <globjects/Texture.h>
#include <globjects/Program.h>
#include <globjects/Shader.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/Buffer.h>
#include <globjects/DebugMessage.h>
#include <globjects/globjects.h>

#include <globjects-base/File.h>

#include <common/ScreenAlignedQuad.h>
#include <common/Camera.h>
#include <common/AxisAlignedBoundingBox.h>
#include <common/AbstractCoordinateProvider.h>
#include <common/WorldInHandNavigation.h>
#include <common/VertexDrawable.h>

#include <common/ContextFormat.h>
#include <common/Context.h>
#include <common/Window.h>
#include <common/WindowEventHandler.h>
#include <common/events.h>

#include <ExampleWindowEventHandler.h>



class EventHandler : public ExampleWindowEventHandler, AbstractCoordinateProvider
{
public:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 texCoord;
        int side;
    };

    EventHandler()
    {
        m_aabb.extend(glm::vec3(-8.f, -1.f, -8.f));
        m_aabb.extend(glm::vec3(8.f, 1.f, 8.f));

        m_camera = Camera(glm::vec3(0.0, 1.0, -1.0)*2.f, glm::vec3(), glm::vec3(0.0, 1.0, 0.0));

        m_nav.setCamera(&m_camera);
        m_nav.setCoordinateProvider(this);
        m_nav.setBoundaryHint(m_aabb);
    }

    virtual ~EventHandler()
    {
    }

    void createTextures();
    void createGeometry();

    virtual void initialize(Window & window) override
    {
        ExampleWindowEventHandler::initialize(window);

        glo::DebugMessage::enable();

        if (!glo::hasExtension(gl::GLextension::GL_NV_bindless_texture))
        {
            glo::critical() << "Blindess textures are not supported";

            window.close();

            return;
        }

        glo::ref_ptr<glo::State> state = new glo::State;
        state->enable(gl::GL_CULL_FACE);
        state->clearColor(0.2f, 0.3f, 0.4f, 1.f);

        createGeometry();
        createTextures();

        m_program = new glo::Program;
        m_program->attach(
            glo::Shader::fromFile(gl::GL_VERTEX_SHADER, "data/bindless-textures/shader.vert"),
            glo::Shader::fromFile(gl::GL_FRAGMENT_SHADER, "data/bindless-textures/shader.frag")
        );

        std::array<glo::TextureHandle, std::tuple_size<decltype(m_textures)>::value> handles;
        for (unsigned i = 0; i < m_textures.size(); ++i)
        {
            handles[i] = m_textures[i]->makeResident();
        }
        m_program->setUniform("textures", handles);

        window.addTimer(0, 0);
    }
    
    virtual void framebufferResizeEvent(ResizeEvent & event) override
    {
        int width = event.width();
        int height = event.height();

        gl::glViewport(0, 0, width, height);


        m_camera.setViewport(width, height);
    }

    virtual void paintEvent(PaintEvent &) override
    {
        gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);


        m_program->setUniform("projection", m_camera.viewProjection());

        m_program->use();
        m_drawable->draw();
        m_program->release();
    }

    virtual void timerEvent(TimerEvent & event) override
    {        
        event.window()->repaint();
    }

    virtual void keyPressEvent(KeyEvent & event) override
    {
        if (event.key()==GLFW_KEY_F5)
            glo::File::reloadAll();
        else if (event.key()==GLFW_KEY_SPACE)
        {
            m_nav.reset();
            event.window()->repaint();
        }
    }

    virtual void mousePressEvent(MouseEvent & event) override
    {
        switch (event.button())
        {
            case GLFW_MOUSE_BUTTON_LEFT:
                m_nav.panBegin(event.pos());
                event.accept();
                break;

            case GLFW_MOUSE_BUTTON_RIGHT:
                m_nav.rotateBegin(event.pos());
                event.accept();
                break;
        }
    }
    virtual void mouseMoveEvent(MouseEvent & event) override
    {
        switch (m_nav.mode())
        {
            case WorldInHandNavigation::PanInteraction:
                m_nav.panProcess(event.pos());
                event.accept();
                break;

            case WorldInHandNavigation::RotateInteraction:
                m_nav.rotateProcess(event.pos());
                event.accept();

            default:
                break;
        }
    }
    virtual void mouseReleaseEvent(MouseEvent & event) override
    {
        switch (event.button())
        {
            case GLFW_MOUSE_BUTTON_LEFT:
                m_nav.panEnd();
                event.accept();
                break;

            case GLFW_MOUSE_BUTTON_RIGHT:
                m_nav.rotateEnd();
                event.accept();
                break;
        }
    }

    virtual void scrollEvent(ScrollEvent & event) override
    {
        if (WorldInHandNavigation::NoInteraction != m_nav.mode())
            return;

        m_nav.scaleAtMouse(event.pos(), -event.offset().y * 0.1f);
        event.accept();
    }

    virtual float depthAt(const glm::ivec2 & windowCoordinates) const override
    {
        return AbstractCoordinateProvider::depthAt(m_camera, gl::GL_DEPTH_COMPONENT, windowCoordinates);
    }

    virtual glm::vec3 objAt(const glm::ivec2 & windowCoordinates) const override
    {
        return unproject(m_camera, static_cast<gl::GLenum>(gl::GL_DEPTH_COMPONENT), windowCoordinates);
    }

    virtual glm::vec3 objAt(const glm::ivec2 & windowCoordinates, const float depth) const override
    {
        return unproject(m_camera, depth, windowCoordinates);
    }

    virtual glm::vec3 objAt(
        const glm::ivec2 & windowCoordinates
    ,   const float depth
    ,   const glm::mat4 & viewProjectionInverted) const override
    {
        return unproject(m_camera, viewProjectionInverted, depth, windowCoordinates);
    }

protected:
    Camera m_camera;

    WorldInHandNavigation m_nav;
    AxisAlignedBoundingBox m_aabb;

    std::array<glo::ref_ptr<glo::Texture>, 4> m_textures;
    glo::ref_ptr<glo::Program> m_program;
    glo::ref_ptr<VertexDrawable> m_drawable;
};


/** This example shows ... .
*/
int main(int /*argc*/, char* /*argv*/[])
{
    glo::info() << "Usage:";
    glo::info() << "\t" << "ESC" << "\t\t" << "Close example";
    glo::info() << "\t" << "ALT + Enter" << "\t" << "Toggle fullscreen";
    glo::info() << "\t" << "F11" << "\t\t" << "Toggle fullscreen";
    glo::info() << "\t" << "F5" << "\t\t" << "Reload shaders";
    glo::info() << "\t" << "Space" << "\t\t" << "Reset camera";
    glo::info() << "\t" << "Left Mouse" << "\t" << "Pan scene";
    glo::info() << "\t" << "Right Mouse" << "\t" << "Rotate scene";
    glo::info() << "\t" << "Mouse Wheel" << "\t" << "Zoom scene";

    ContextFormat format;
    format.setVersion(3, 0);

    Window window;

    window.setEventHandler(new EventHandler());

    if (window.create(format, "Bindless Texture Example"))
    {
        window.context()->setSwapInterval(Context::NoVerticalSyncronization);

        window.show();

        return MainLoop::run();
    }
    else
    {
        return 1;
    }
}

void EventHandler::createTextures()
{
    const std::array<glm::bvec3, 4> masks = { {
        glm::bvec3(true, false, false),
        glm::bvec3(false, true, false),
        glm::bvec3(false, false, true),
        glm::bvec3(true, true, false)
    } };

    for (unsigned i = 0; i < m_textures.size(); ++i)
    {
        glo::Texture* texture = glo::Texture::createDefault(gl::GL_TEXTURE_2D);

        static const int w = 512;
        static const int h = 512;
        using uchar = unsigned char;
        struct RGBA { uchar r; uchar g; uchar b; uchar a; };
        std::array<RGBA, w*h> data;

        for (unsigned j = 0; j < std::tuple_size<decltype(data)>::value; ++j)
        {
            int x = j%w;
            int y = j/w;

            glm::vec2 pos(float(x)/w, float(y)/h);

            float h = glm::perlin(pos*16.f+glm::vec2(float(i)*3.f));

            glm::vec3 color = glm::vec3(masks[i%std::tuple_size<decltype(masks)>::value]) * h;

            glm::ivec3 icolor = glm::ivec3(color*255.f);

            data[j] = RGBA{ static_cast<uchar>(icolor.r), static_cast<uchar>(icolor.g), static_cast<uchar>(icolor.b), 255 };
        }

        texture->image2D(0, gl::GL_RGBA8, w, h, 0, gl::GL_RGBA, gl::GL_UNSIGNED_BYTE, data.data());

        m_textures[i] = texture;
    }
}

void EventHandler::createGeometry()
{
    std::array<glm::vec3, 8> points = { {
        glm::vec3(glm::sin(glm::radians(0.f)), 0.0, glm::cos(glm::radians(0.f))),
        glm::vec3(glm::sin(glm::radians(120.f)), 0.0, glm::cos(glm::radians(120.f))),
        glm::vec3(glm::sin(glm::radians(240.f)), 0.0, glm::cos(glm::radians(240.f))),
        glm::vec3(0.0, glm::sqrt(glm::pow(2.f/glm::cos(glm::radians(30.f))-1.f, 2.f)), 0.0)
    } };

    std::array<Vertex, 6> vertices = { {
        Vertex{ points[0], glm::vec2(0.0, 0.0), 0 },
        Vertex{ points[1], glm::vec2(1.0, 0.0), 0 },
        Vertex{ points[3], glm::vec2(0.5, 1.0), 0 },

        Vertex{ points[2], glm::vec2(0.0, 0.0), 1 },
        Vertex{ points[0], glm::vec2(1.0, 0.0), 2 },
        Vertex{ points[1], glm::vec2(0.5, 1.0), 3 }
    } };

    m_drawable = new VertexDrawable(vertices, gl::GL_TRIANGLE_STRIP);

    m_drawable->setFormats({
        Format(3, gl::GL_FLOAT, offsetof(Vertex, position)),
        Format(2, gl::GL_FLOAT, offsetof(Vertex, texCoord)),
        FormatI(1, gl::GL_INT, offsetof(Vertex, side))
    });
    m_drawable->enableAll();
}
