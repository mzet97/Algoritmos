This is a [Next.js](https://nextjs.org) project bootstrapped with [`create-next-app`](https://nextjs.org/docs/app/api-reference/cli/create-next-app).

## Getting Started

First, run the development server:

```bash
npm run dev
# or
yarn dev
# or
pnpm dev
# or
bun dev
```

Open [http://localhost:3000](http://localhost:3000) with your browser to see the result.

You can start editing the page by modifying `app/page.tsx`. The page auto-updates as you edit the file.

This project uses [`next/font`](https://nextjs.org/docs/app/building-your-application/optimizing/fonts) to automatically optimize and load [Geist](https://vercel.com/font), a new font family for Vercel.

## Learn More

To learn more about Next.js, take a look at the following resources:

- [Next.js Documentation](https://nextjs.org/docs) - learn about Next.js features and API.
- [Learn Next.js](https://nextjs.org/learn) - an interactive Next.js tutorial.

You can check out [the Next.js GitHub repository](https://github.com/vercel/next.js) - your feedback and contributions are welcome!

## Code execution (Judge0, self-hosted, ephemeral)

The exercise widget posts `{ linguagem, codigo }` to `/api/run`, which proxies
to a self-hosted Judge0 instance. No student code is compiled or executed by the
Next.js server in production, and nothing is stored server-side.

Environment:

```bash
JUDGE0_URL=http://localhost:2358   # required in production
JUDGE0_TOKEN=...                   # optional, if the instance requires auth
ALLOW_LOCAL_EXEC=1                 # offline dev only, never in production
```

Language IDs used: C=50, C++=54, Rust=73, Go=60, Python=71 (Judge0 CE defaults).
Limits per submission: 5s CPU, 10s wall; memory default 128 MB (address space),
4 GB for Go. Without `JUDGE0_URL` (and without `ALLOW_LOCAL_EXEC=1`) the API
returns 503.

Judge0 CE 1.13.1 runs on the host via rootless podman
(`~/judge0/docker-compose.yml`, passwords in `~/judge0/judge0.conf`), reusing
the repo's LocalStack-style pattern (`distrobox-host-exec ...` when the host
bridge works, otherwise the host user's podman socket directly). Two host-side
caveats, both verified while getting this up:

- The image ships files owned by high UIDs, which rootless podman cannot map
  with the default `mzet:524288:65536` range. Workaround without touching
  `/etc/subuid`: `~/.config/containers/storage.conf` with
  `ignore_chown_errors = "true"`.
- Rootless containers lack cgroups, so isolate's `--cg` mode fails and every
  submission ends in `Internal Error`. Fix: per-process limits
  (`enable_per_process_and_thread_* = true`, the server default in
  `judge0.conf`, also sent explicitly by `/api/run`).
- Go 1.13 reserves gigabytes of virtual address space when compiling, so the
  server runs with `MAX_MEMORY_LIMIT=4194304` and `/api/run` requests 4 GB
  (address space, not RSS) for Go only.

## Deploy on Vercel

The easiest way to deploy your Next.js app is to use the [Vercel Platform](https://vercel.com/new?utm_medium=default-template&filter=next.js&utm_source=create-next-app&utm_campaign=create-next-app-readme) from the creators of Next.js.

Check out our [Next.js deployment documentation](https://nextjs.org/docs/app/building-your-application/deploying) for more details.
