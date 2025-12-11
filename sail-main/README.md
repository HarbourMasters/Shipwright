## What is this?

Sail is a networking protocol designed to facilitate remote control of the Ship
of Harkinian client. It is intended to be utilized alongside a Sail server, for
which we provide a few straightforward implementations here in this repo. The
current implementations available allow integration with Twitch chat and SAMMI
Bot, feel free to contribute your own!

## How to

- Download
  [the associated SoH Build](https://github.com/HarbourMasters/Shipwright/pull/3073)
- Install [Deno](https://deno.land/manual@v1.34.3/getting_started/installation)
- Clone this repository
- Run either the custom or JSON sail, the differences are explained below

```sh
cd <path to this repo>/examples
deno run --allow-net --allow-read twitch_json_sail.ts
```

## Sail Types

Out of the box both of the twitch types are configured with the same
commands/rewards, primarily to show you an example of how to configure them.

### Twitch JSON Sail

The JSON sail is configuration driven by a JSON file, while it is easier to
configure and get running, it is less flexible than the custom sail. I'd start
here until you need to do something that the JSON sail doesn't support.

### Twitch Custom Sail

The custom sail is a TypeScript file that is compiled and run by Deno. It is
more flexible than the JSON sail, but requires a bit more knowledge of
programming to configure to your liking. Your limit is your imagination here.

### SAMMI Sail

The SAMMI sail is meant to be used with the SAMMI bot, it simply redirects
traffic between your SAMMI bot and SoH. You can configure the sail with env
variables to point to your SAMMI's webhook endpoint with SAMMI_WEBHOOK_URL and
have SAMMI point to the Sail's endpoint at http://localhost:43383
