# 🍞 Tost Engine TRELLIS 2 Unreal Engine Plugin

A comprehensive Unreal Engine plugin that integrates TRELLIS 2 AI model for generating high-quality 3D models from 2D images. This plugin provides a user-friendly interface within the Unreal Editor for seamless 3D content creation.

![Screenshot 2026-01-10 021016](https://github.com/user-attachments/assets/dff48f54-1e94-4483-9c72-458eaef34e04)

## Features

### Core Functionality
- **Image-to-3D Generation**: Convert 2D images into detailed 3D models using advanced AI technology
- **TRELLIS 2 Integration**: Powered by the state-of-the-art TRELLIS 2 model for native and compact structured latents
- **Dual Mode Support**: Local and remote API modes for flexible deployment
- **Webhook Server**: Built-in HTTP server for receiving generation notifications and status updates

### User Interface
- **Intuitive Editor Window**: Clean, professional UI integrated into Unreal Editor
- **Real-time Status Updates**: Live progress tracking during generation
- **Configurable Parameters**: Extensive customization options for generation settings
- **Automatic Import**: Seamless GLB file download and import into Unreal scenes

### Technical Features
- **HTTP Client**: Robust HTTP communication for API interactions
- **Webhook Handling**: Automatic processing of generation completion notifications
- **Background Processing**: Non-blocking generation with progress feedback
- **Error Handling**: Comprehensive error reporting and recovery
- **Firewall Management**: Automatic port configuration and firewall rule setup

### Generation Parameters
- **Seed Control**: Reproducible results with configurable random seeds
- **Resolution Settings**: Multiple output resolutions (1K, 2K, 4K)
- **Background Removal**: Optional automatic background removal from input images
- **Guidance Parameters**: Fine-tune generation with SS, Shape SLAT, and Tex SLAT guidance strengths
- **Sampling Steps**: Adjustable sampling steps for quality vs speed trade-off

## Requirements

- **Unreal Engine**: Version 5.7 or later
- **Operating System**: Windows 10/11 (64-bit)
- **Visual Studio**: 2022 with C++ development tools
- **Internet Connection**: Required for remote API mode
- **Local API Server**: Optional, for local processing mode

## Installation

1. **Download the Plugin**:
   - Clone or download the plugin source code
   - Place the `TostEngineTrellis2` folder in your project's `Plugins` directory

2. **Enable the Plugin**:
   - Open your Unreal Engine project
   - Go to `Edit > Plugins`
   - Search for "TostEngine TRELLIS 2"
   - Enable the plugin and restart the editor

3. **Build the Plugin** (if not pre-built):
   - Follow the build instructions below

## Building from Source

### Prerequisites
- Unreal Engine 5.7 installed
- Visual Studio 2022 with the following workloads:
  - Game development with C++
  - Windows 10/11 SDK

### Build Steps

1. **Open Command Prompt** as Administrator

2. **Navigate to Project Directory**:
   ```
   cd c:\Users\PC\Documents\content\unreal\TostEngineTrellis2
   ```

3. **Run the Build Command**:
   ```
   "C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="c:\Users\PC\Documents\content\unreal\TostEngineTrellis2\TostEngineTrellis2.uplugin" -Package="c:\Users\PC\Documents\content\unreal\TostEngineTrellis2\Build" -Rocket
   ```

4. **Wait for Completion**:
   - The build process will compile for both Development and Shipping configurations
   - This may take several minutes depending on your system

5. **Verify Build Success**:
   - Check that the `Build` directory contains the packaged plugin files
   - Look for "BUILD SUCCESSFUL" in the output

### Troubleshooting Build Issues

- **Missing Dependencies**: Ensure all required Unreal Engine components are installed
- **Visual Studio Errors**: Verify VS 2022 is properly configured for UE development
- **Path Issues**: Use absolute paths in the build command
- **Firewall**: Some antivirus software may interfere with the build process

## Usage

### Basic Workflow

1. **Open the Plugin Window**:
   - In Unreal Editor: `Window > TostEngine TRELLIS 2`

2. **Configure Settings**:
   - Select input image
   - Choose local or remote mode
   - Configure API URLs and authentication
   - Adjust generation parameters

3. **Enable Webhook Server** (Recommended):
   - Check "Enable Webhook Server" for automatic notifications
   - Ensure port 8080 is available and firewall allows access

4. **Generate 3D Model**:
   - Click "Generate 3D Model"
   - Monitor progress in the status area
   - Wait for completion and automatic import

### Advanced Configuration

#### Local Mode Setup
- Set "Local Upload URL" (e.g., `http://localhost:9000`)
- Set "Local API URL" (e.g., `http://localhost:8000`)
- Ensure local TRELLIS 2 server is running

#### Remote Mode Setup
- Obtain API token from your TRELLIS 2 service provider
- Enter "Auth Token" and "Worker ID"
- Webhook URL is automatically configured when webhook server is enabled

#### Webhook Server Configuration
- Default port: 8080
- Accessible endpoints:
  - `GET /status` - Server status check
  - `POST /webhook` - Webhook notifications
- Automatically binds to all network interfaces

### Firewall Configuration

For external access to the webhook server:

**Add Rule:**
```
netsh advfirewall firewall add rule name="Allow HTTP on Port 8080" dir=in action=allow protocol=TCP localport=8080
```

**Remove Rule:**
```
netsh advfirewall firewall delete rule name="Allow HTTP on Port 8080"
```

## API Reference

### Webhook Payload Format
```json
{
  "output": {
    "status": "DONE",
    "result": "https://example.com/model.glb"
  }
}
```

### Status Response
```json
{
  "status": "Webhook server is running",
  "port": 8080
}
```

## Development

### Project Structure
```
TostEngineTrellis2/
├── Source/
│   ├── TostEngineTrellis2/          # Runtime module
│   │   ├── Private/                 # Implementation files
│   │   └── Public/                  # Headers
│   └── TostEngineTrellis2Editor/    # Editor module
│       ├── Private/                 # Editor implementation
│       └── Public/                  # Editor headers
├── Config/                          # Plugin configuration
├── Resources/                       # Icons and assets
└── TostEngineTrellis2.uplugin       # Plugin descriptor
```

### Key Classes
- `UTostEngineWebhookServer`: HTTP server for webhooks
- `UTostEngineHttpClient`: HTTP client for API communication
- `STostEngineTrellis2Window`: Main editor UI
- `UTostEngineTrellis2Functions`: Core functionality

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Troubleshooting

### Common Issues

**Plugin Won't Load**
- Ensure UE 5.7+ is installed
- Check plugin dependencies
- Verify build configuration

**Generation Fails**
- Check API credentials
- Verify network connectivity
- Review error logs in editor output

**Webhook Server Issues**
- Confirm port 8080 is not in use
- Check firewall settings
- Verify local IP address

**Build Errors**
- Clean and rebuild
- Update Visual Studio
- Check UE installation integrity

### Logs and Debugging
- Enable verbose logging in editor
- Check `Saved/Logs` directory
- Use `UE_LOG` statements for custom debugging

## License

MIT License


